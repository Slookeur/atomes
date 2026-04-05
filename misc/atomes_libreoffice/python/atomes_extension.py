# -*- coding: utf-8 -*-
"""Extension atomes pour LibreOffice.

Insère des fichiers .apf dans tout document LibreOffice.
Le fichier source est embarqué dans le stockage ODF et une image
de prévisualisation (ou l'icône atomes) est insérée.

Interactions :
  • Menu  atomes → Insérer / Ouvrir
  • Clic sur l'image insérée       (OnClick — second clic)
  • Clic droit → Ouvrir avec atomes  (XContextMenuInterceptor)
  • Double-clic → Ouvre atomes       (XMouseClickHandler)
"""
#
# The following helps to traceback error, at any point, 
# simply feed back the result to any AI assitant to get to the point
# 
# import traceback
# traceback.print_exc()
# 

import os
import subprocess
import traceback
import tempfile
import uuid
import uno
import unohelper
from com.sun.star.awt import XMouseClickHandler, Size
from com.sun.star.embed import ElementModes
from com.sun.star.ui import XContextMenuInterceptor
from com.sun.star.ui.ContextMenuInterceptorAction import IGNORED, EXECUTE_MODIFIED

try:
    from PIL import Image
except ImportError:
    Image = None

from atomes_i18n import _

# ── Constants ──────────────────────────────────────────────────────────
EXTENSION_ID       = "fr.ipcms.atomes.extension"
ATOMES_STORAGE     = "ObjectReplacements"
ATOMES_PREFIX      = "AtomesFile_"
ATOMES_DESCRIPTION = "AtomesFile:"

# Session-level references (prevent GC)
_mouse_handlers   = {}
_ctx_interceptors = {}

# ══════════════════════════════════════════════════════════════════════
# Low-level helpers
# ══════════════════════════════════════════════════════════════════════

def _lo_ctx():
    return uno.getComponentContext()


def _get_extension_dir():
    pip = _lo_ctx().ServiceManager.createInstance("com.sun.star.deployment.PackageInformationProvider")
    return uno.fileUrlToSystemPath(pip.getPackageLocation(EXTENSION_ID))


def _get_document():
    try:
        desktop = _lo_ctx().ServiceManager.createInstance("com.sun.star.frame.Desktop")
        return desktop.getCurrentComponent()
    except Exception:
        return None


def _get_draw_page(doc):
    if doc.supportsService("com.sun.star.text.TextDocument"):
        return doc.DrawPage
    if doc.supportsService("com.sun.star.sheet.SpreadsheetDocument"):
        return doc.getCurrentController().getActiveSheet().DrawPage
    if doc.supportsService("com.sun.star.presentation.PresentationDocument"):
        return doc.getCurrentController().getCurrentPage()
    if doc.supportsService("com.sun.star.drawing.DrawingDocument"):
        return doc.getCurrentController().getCurrentPage()
    return doc.DrawPage


def _show_message(doc, msg, title, error=False):
    from com.sun.star.awt.MessageBoxType import INFOBOX, ERRORBOX
    from com.sun.star.awt.MessageBoxButtons import BUTTONS_OK
    try:
        toolkit = _lo_ctx().ServiceManager.createInstance("com.sun.star.awt.Toolkit")
        frame   = doc.getCurrentController().getFrame() if doc else None
        peer    = frame.getContainerWindow() if frame else None
        kind    = ERRORBOX if error else INFOBOX
        box     = toolkit.createMessageBox(peer, kind, BUTTONS_OK, title, msg)
        box.execute() 
        box.dispose()
    except Exception as e:
        print(f"Erreur dans _show_message : {e}")
        traceback.print_exc()
        pass


def _make_pv(name, value):
    pv = uno.createUnoStruct("com.sun.star.beans.PropertyValue")
    pv.Name = name; pv.Value = value
    return pv


def _event_props(macro_url):
    return (_make_pv("EventType", "Script"), _make_pv("Script", macro_url))

def list_storage(storage, prefix=""):
    try:
        names = storage.getElementNames()
        for name in names:
            full_path = f"{prefix}{name}"
            # print(full_path)

            try:
                if storage.isStorageElement(name):
                    print(f"[DIR ] {full_path}/")
                    sub = storage.openStorageElement(name, ElementModes.READ)
                    list_storage(sub, full_path + "/")

                elif storage.isStreamElement(name):
                    print(f"[FILE] {full_path}")

                else:
                    print(f"[??? ] {full_path}")

            except Exception:
                pass
    except Exception:
        import traceback
        traceback.print_exc()

# ══════════════════════════════════════════════════════════════════════
# Shape selection helpers
# ══════════════════════════════════════════════════════════════════════

def _get_selected_atomes_shape_from_selection(doc):
    try:
        sel = doc.getCurrentController().getSelection()
        if sel is None:
            return None
        if hasattr(sel, "Name") and sel.Name.startswith(ATOMES_PREFIX):
            return sel
        if hasattr(sel, "Count"):
            for i in range(sel.Count):
                s = sel.getByIndex(i)
                if hasattr(s, "Name") and s.Name.startswith(ATOMES_PREFIX):
                    return s
    except Exception:
        pass
    return None

def _get_selected_atomes_shape_from_description(doc, desc):
    try:
        draw_page = _get_draw_page(doc)
        if draw_page is None:
            return None
        shape_desc = f"{ATOMES_DESCRIPTION}{desc}"
        for i in range(draw_page.getCount()):
            shape = draw_page.getByIndex(i)
            if hasattr(shape, "Description") and shape.Description == shape_desc:
                 return shape
    except Exception:
        pass
    return None

def _stored_name(shape):
    desc = getattr(shape, "Description", "") or ""
    if desc.startswith(ATOMES_DESCRIPTION):
        return desc[len(ATOMES_DESCRIPTION):]
    return None


# ══════════════════════════════════════════════════════════════════════
# ODF storage
# ══════════════════════════════════════════════════════════════════════

def _embed_file(doc, filepath, stored_name, replace=False):
    """
    Écrit un fichier dans le stockage ODF.
    
    - replace=False → création si absent (embed)
    - replace=True  → remplace uniquement si existant
    """
    try:     
        root = doc.getDocumentStorage()
        list_storage(root)
        mode = ElementModes.READWRITE

        # Accès / création du sous-stockage
        if not root.hasByName(ATOMES_STORAGE):
            if replace:
                print("Storage inexistant, impossible de remplacer.")
                return False

        atomes_storage = root.openStorageElement(ATOMES_STORAGE, mode)

        # apf_file_name = f"{ATOMES_STORAGE}/{stored_name}"
        exists = atomes_storage.hasByName(stored_name)
        # Cas remplacement strict
        if replace and not exists:
            print(f"Fichier {stored_name} introuvable pour remplacement.")
            return False

        # Ouverture du stream
        stream_mode = mode | ElementModes.TRUNCATE if exists else mode
        stream = atomes_storage.openStreamElement(stored_name, stream_mode)
        out = stream.getOutputStream()

        with open(filepath, "rb") as fh:
            out.writeBytes(uno.ByteSequence(fh.read()))

        out.closeOutput()

        # Commit obligatoire
        stream = None
        atomes_storage.commit()
        root.commit()

        action = "remplacé" if exists else "ajouté"
        print(f"Fichier {stored_name} {action} avec succès.")
        list_storage(root)
        return True

    except Exception as e:
        print(f"Erreur écriture fichier embarqué : {e}")
        return False

def _resolve_apf_from_shape(shape):
    name = shape.Name or ""

    if not name.startswith(ATOMES_PREFIX):
        return None

    obj_id = name.split("_", 1)[1]
    print(f"resolve :: {obj_id}")

#    return f"{ATOMES_STORAGE}/{obj_id}"
    return obj_id

def _extract_atomes_file(doc, stored_name):
    try:
        root = doc.getDocumentStorage()
        mode = ElementModes.READ
        print("Got Storage")
        print("Root after reload:", list(root.getElementNames()))
        if not root.hasByName(ATOMES_STORAGE):
            print("No ATOMES_STORAGE")
            return None
        print("Looking for storage:", ATOMES_STORAGE)
        print("Root elements:", list(root.getElementNames()))
        atomes_storage = root.openStorageElement(ATOMES_STORAGE, mode)
        if not atomes_storage.hasByName(stored_name):
            return None
        stream  = atomes_storage.openStreamElement(stored_name, mode)
        inp     = stream.getInputStream()
        # apf_file_name = f"{ATOMES_STORAGE}/{stored_name}"
        print("Looking for file:", stored_name)
        print("Available files:", list(atomes_storage.getElementNames()))
        chunks  = []
        buf_ref = uno.ByteSequence(b"\x00" * 65536)
        while True:
            n, chunk = inp.readBytes(buf_ref, 65536)
            if n == 0:
                break
            chunks.append(bytes(chunk))
        inp.closeInput()
        with tempfile.NamedTemporaryFile(suffix=".apf", delete=False) as tmp:
            for c in chunks:
                tmp.write(c)
            return tmp.name
    except Exception as e:
        import traceback
        traceback.print_exc()
        return None

def _list_embedded_files(doc):
    try:
        root = doc.getDocumentStorage()
        if not root.hasByName(ATOMES_STORAGE):
            return []
        atomes_storage = root.openStorageElement(ATOMES_STORAGE, ElementModes.READ)
        return [n for n in atomes_storage.getElementNames() if n.endswith(".apf")]
    except Exception:
        return []


# ══════════════════════════════════════════════════════════════════════
# UNO event handlers
# ══════════════════════════════════════════════════════════════════════

class AtomesMouseHandler(unohelper.Base, XMouseClickHandler):
    """Intercepts double-click on atomes shapes and opens atomes."""
    def __init__(self, doc):
        self.doc = doc

    def mousePressed(self, event):
        if event.ClickCount == 2:
            return on_atomes_click()
        return False

    def mouseReleased(self, event):
        return False


class AtomesContextMenuInterceptor(unohelper.Base, XContextMenuInterceptor):
    """Adds 'Open with atomes' to the context menu for atomes shapes."""
    def __init__(self, doc):
        self.doc = doc

    def notifyContextMenuExecute(self, event):
        print("Interception du menu contextuel...")
        try:
            shape = _get_selected_atomes_shape_from_selection(self.doc)
            print(f"Objet sélectionné : {shape}")
            if shape is None:
                print("Aucun objet Atomes sélectionné.")
                return IGNORED
            menu    = event.ActionTriggerContainer
            trigger = _lo_ctx().ServiceManager.createInstance("com.sun.star.ui.ActionTrigger")
            trigger.Text = _("context_menu_open")
            trigger.CommandURL = ("vnd.sun.star.script:atomes_extension.py$open_from_context_menu?language=Python&location=share")
            menu.insertByIndex(0, trigger)
            print("Élément ajouté au menu contextuel.")
            return EXECUTE_MODIFIED
        except Exception:
            return IGNORED


def _register_handlers(doc):
    """Register session-level mouse + context-menu handlers (idempotent)."""
    if doc is None:
        return
    try:
        key = doc.getURL() or str(id(doc))
        ctrl = doc.getCurrentController()
        if ctrl is None:
            print("Contrôleur introuvable.")
            return

        if key not in _ctx_interceptors:
            i = AtomesContextMenuInterceptor(doc)
            try:
                if hasattr(ctrl, "addContextMenuInterceptor"):
                    ctrl.addContextMenuInterceptor(i)
                    _ctx_interceptors[key] = i
                    print("Intercepteur de menu contextuel enregistré.")
                else:
                    print("Le contrôleur ne supporte pas addContextMenuInterceptor.")
                    traceback.print_exc()
            except  Exception as e:
                print(f"Erreur lors de l'ajout de l'intercepteur : {e}")
                traceback.print_exc()

        if key not in _mouse_handlers:
            h = AtomesMouseHandler(doc)
            ctrl.addMouseClickHandler(h)
            _mouse_handlers[key] = h

    except Exception as e:
       print(f"Erreur dans _register_handlers : {e}")
       traceback.print_exc()


# ══════════════════════════════════════════════════════════════════════
# Core: open an embedded file
# ══════════════════════════════════════════════════════════════════════

def _open_embedded_file(doc, stored_name, on_click):
    if not stored_name:
        _show_message(doc, _("invalid_file_name"), _("error_title"), error=True)
        return
    tmp = _extract_atomes_file(doc, stored_name)
    if tmp is None:
        _show_message(doc, f"{_('file_not_found')}:\n{stored_name}", _("error_title"), error=True)
        return
    try:
        # Génère un ID unique pour l'objet
        if on_click:
            shape = _get_selected_atomes_shape_from_selection(doc)
        else:
            shape = _get_selected_atomes_shape_from_description(doc, stored_name)
        uid = shape.Name.split("_")[-1] if shape else "unknown"
        output_image = f"/tmp/atomes_update_{uid}.png"
        result = subprocess.run(["atomes", "--libreoffice", "--output", output_image, tmp], capture_output=True, text=True)
        #print(f"result.returncode= {result.returncode}")
        #print(f"Sortie d'atomes : {result.stdout}")
        #print(f"Erreur d'atomes : {result.stderr}")
        if result.returncode == 0: 
            if os.path.exists(output_image):
                # Met à jour l'objet graphique avec la nouvelle image
                shape.GraphicURL = uno.systemPathToFileUrl(output_image)
                with Image.open(output_image) as img:
                    width, height = img.size
                width_twips = int(width * 1440 / 96 )  # Approximation moyenne
                height_twips = int(height * 1440 / 96)
                shape.Size = Size(width_twips, height_twips)
                # Nettoie le fichier temporaire
                if os.path.exists(output_image):
                    os.unlink(output_image)
            else:
                _show_message(doc, _("image_update_failed"), _("error_title"), error=False)
            # Update apf content in LibreOffice document
            if not _embed_file(doc, tmp, stored_name, replace=True):
                _show_message(doc, _("embed_failed"), _("error_title"), error=True)
        else:
            _show_message(doc, _("update_failed"), _("error_title"), error=True)
    except Exception as e:
        _show_message(doc, f"{_('open_atomes_failed')}\n{e}", _("error_title"), error=True)


# ══════════════════════════════════════════════════════════════════════
# Selection dialog (multiple embedded files)
# ══════════════════════════════════════════════════════════════════════

def _selection_dialog(files, doc):
    try:
        ctx = _lo_ctx()
        dm  = ctx.ServiceManager.createInstance("com.sun.star.awt.UnoControlDialogModel")
        dm.Width = 250; dm.Height = 130; dm.Title = _("select_atomes_title")

        lbl = dm.createInstance("com.sun.star.awt.UnoControlFixedTextModel")
        lbl.PositionX = 8; lbl.PositionY = 8; lbl.Width = 234; lbl.Height = 24
        lbl.Label = _("select_atomes_label"); lbl.MultiLine = True
        dm.insertByName("lbl", lbl)

        lb = dm.createInstance("com.sun.star.awt.UnoControlListBoxModel")
        lb.PositionX = 8; lb.PositionY = 36; lb.Width = 234; lb.Height = 60
        lb.StringItemList = tuple(files); lb.SelectedItems = (0,)
        dm.insertByName("lb", lb)

        btn = dm.createInstance("com.sun.star.awt.UnoControlButtonModel")
        btn.PositionX = 170; btn.PositionY = 110; btn.Width = 72; btn.Height = 16
        btn.Label = _("ok"); btn.PushButtonType = 1
        dm.insertByName("btn", btn)

        dlg = ctx.ServiceManager.createInstance("com.sun.star.awt.UnoControlDialog")
        dlg.setModel(dm)
        tk = ctx.ServiceManager.createInstance("com.sun.star.awt.Toolkit")
        dlg.createPeer(tk, None)

        if dlg.execute() == 1:
            idx = dlg.getControl("lb").getSelectedItemPos()
            dlg.dispose()
            return files[idx] if 0 <= idx < len(files) else files[0]
        dlg.dispose()
    except Exception:
        pass
    return files[0] if files else None


# ══════════════════════════════════════════════════════════════════════
# Exported macros
# ══════════════════════════════════════════════════════════════════════

def insert_atomes_file(*args):
    """Menu: atomes → Insérer un fichier / Insert a file."""
    doc = _get_document()
    if doc is None:
        return None

    
    # File picker
    fp = _lo_ctx().ServiceManager.createInstance("com.sun.star.ui.dialogs.FilePicker")
    fp.setTitle(_("insert_title"))
    fp.appendFilter(_("select_file_filter"), "*.apf")
    fp.appendFilter(_("all_files"), "*.*")
    fp.setCurrentFilter(_("select_file_filter"))
    if fp.execute() != 1:
        return None
    files = fp.getFiles()
    if not files:
        return None

    apf_path     = uno.fileUrlToSystemPath(files[0])
    apf_basename = os.path.basename(apf_path)

    # Render preview
    png_path = None; image_ok = False

    try:
        with tempfile.NamedTemporaryFile(suffix=".png", delete=False, prefix="atomes_") as tmp:
            png_path = tmp.name
        result = subprocess.run(
            ["atomes", "--render-png", apf_path, "--output", png_path],
            timeout=120, capture_output=True)
        if result.returncode == 0 and os.path.exists(png_path) and os.path.getsize(png_path) > 0:
            image_ok = True
    except Exception:
        image_ok = False

    # Fallback: bundled SVG icon
    if not image_ok:
        try:
            icon = os.path.join(_get_extension_dir(), "icons", "atomes.svg")
            png_path = icon if os.path.exists(icon) else None
        except Exception:
            png_path = None

    # Insert GraphicObjectShape
    try:
        draw_page = _get_draw_page(doc)
        shape     = doc.createInstance("com.sun.star.drawing.GraphicObjectShape")
        draw_page.add(shape)
        shape.Size        = Size(6000, 6000)   # 6 cm × 6 cm default
        if png_path and os.path.exists(png_path):
            shape.GraphicURL = uno.systemPathToFileUrl(png_path)
            if Image is not None:
                with Image.open(png_path) as img:
                    width, height = img.size
                width_twips  = int(width * 1440 / 96 )  # Approximation moyenne
                height_twips = int(height * 1440 / 96)
                shape.Size = Size(width_twips, height_twips)
        uid               = uuid.uuid4().hex[:12]
        # f{ATOMES
        unique_name       = f"{uuid.uuid4().hex[:8]}_{apf_basename}"
        shape.Name        = f"{ATOMES_PREFIX}{unique_name}"
        package_url = f"vnd.sun.star.Package:ObjectReplacements/{unique_name}"
        shape.Description = f"{ATOMES_DESCRIPTION}{package_url}"
        shape.Title       = f"atomes — {apf_basename}"
        shape.setPropertyValue("Hyperlink", f"vnd.sun.star.Package:{unique_name}")
        macro_url = ("vnd.sun.star.script:atomes_extension.py$on_atomes_click?language=Python&location=share")
        try:
            shape.Events.replaceByName("OnClick", _event_props(macro_url))
        except Exception:
            pass
    except Exception as e:
        _show_message(doc, str(e), _("error_title"), error=True)
        return None

    # Embed .apf in ODF storage
    if not _embed_file(doc, apf_path, unique_name, replace=False):
        _show_message(doc, _("embed_failed"), _("error_title"), error=True)

    # Register session handlers
    _register_handlers(doc)

    # Cleanup temp PNG
    if image_ok and png_path and os.path.exists(png_path):
        try: os.unlink(png_path)
        except Exception: pass

    return None


def open_atomes_file(*args):
    """Menu: atomes → Ouvrir un fichier / Open a file."""
    doc = _get_document()
    if doc is None:
        return None
    embedded = _list_embedded_files(doc)
    if not embedded:
        _show_message(doc, _("no_atomes_file"), _("open_title"), error=False)
        return None
    chosen = embedded[0] if len(embedded) == 1 else _selection_dialog(embedded, doc)
    if chosen:
        _open_embedded_file(doc, chosen, False)
    return None


def on_atomes_click(*args):
    """Mouse double click callback on atomes shapes."""
    try:
        doc = _get_document()
        if doc is None:
            return True
        shape = _get_selected_atomes_shape_from_selection(doc)
        if shape:
#            name = _stored_name(shape)
            name = _resolve_apf_from_shape(shape)
            if name:
                _open_embedded_file(doc, name, True)
    except Exception as e:
        print(f"Erreur dans on_atomes_click : {e}")
        traceback.print_exc()

    return True


def open_from_context_menu(*args):
    """Context-menu → Open with atomes."""
    return on_atomes_click(*args)


g_exportedScripts = (
    insert_atomes_file,
    open_atomes_file,
    on_atomes_click,
    open_from_context_menu,
)
