# -*- coding: utf-8 -*-
"""Internationalisation (FR / EN) — Extension atomes pour LibreOffice."""

import uno

STRINGS = {
    "fr": {
        "insert_title":        "Insérer un fichier projet atomes",
        "open_title":          "Ouvrir un fichier projet atomes",
        "select_file_filter":  "Fichiers projet atomes (*.apf)",
        "all_files":           "Tous les fichiers (*.*)",
        "error_title":         "Erreur — Extension atomes",
        "no_doc":              "Aucun document ouvert.",
        "no_atomes_file":      "Aucun fichier atomes trouvé dans ce document.",
        "render_failed":       "Impossible de générer l'aperçu.\nVérifiez qu'atomes est installé et accessible.",
        "open_atomes_failed":  "Impossible d'ouvrir le fichier avec atomes.",
        "select_atomes_title": "Choisir un fichier atomes",
        "select_atomes_label": "Plusieurs fichiers atomes sont présents dans ce document.\nChoisissez celui à ouvrir :",
        "context_menu_open":   "Ouvrir avec atomes",
        "embed_failed":        "Impossible d'embarquer le fichier dans le document.",
        "ok":                  "OK",
        "cancel":              "Annuler",
    },
    "en": {
        "insert_title":        "Insert an atomes project file",
        "open_title":          "Open an atomes project file",
        "select_file_filter":  "atomes project files (*.apf)",
        "all_files":           "All Files (*.*)",
        "error_title":         "Error — atomes Extension",
        "no_doc":              "No document is open.",
        "no_atomes_file":      "No atomes file found in this document.",
        "render_failed":       "Cannot generate preview.\nPlease check that atomes is installed and accessible.",
        "open_atomes_failed":  "Cannot open the file with atomes.",
        "select_atomes_title": "Select an atomes File",
        "select_atomes_label": "Several atomes files are present in this document.\nSelect the one to open:",
        "context_menu_open":   "Open with atomes",
        "embed_failed":        "Cannot embed the file into the document.",
        "ok":                  "OK",
        "cancel":              "Cancel",
    },
}

def _detect_locale():
    try:
        ctx  = uno.getComponentContext()
        prov = ctx.ServiceManager.createInstance(
            "com.sun.star.configuration.ConfigurationProvider")
        arg       = uno.createUnoStruct("com.sun.star.beans.PropertyValue")
        arg.Name  = "nodepath"
        arg.Value = "/org.openoffice.Setup/L10N"
        acc = prov.createInstanceWithArguments(
            "com.sun.star.configuration.ConfigurationAccessService", (arg,))
        locale = acc.getByName("ooLocale") or ""
        if locale.lower().startswith("fr"):
            return "fr"
    except Exception:
        pass
    return "en"

_LOCALE = None

def _(key):
    global _LOCALE
    if _LOCALE is None:
        _LOCALE = _detect_locale()
    return STRINGS.get(_LOCALE, STRINGS["en"]).get(key, STRINGS["en"].get(key, key))
