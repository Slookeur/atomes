import unohelper
from com.sun.star.task import XJobExecutor
import sys
import os

# ajoute le dossier courant au sys.path
ext_path = os.path.dirname(__file__)
if ext_path not in sys.path:
    sys.path.append(ext_path)

import atomes_extension

class AtomesService(unohelper.Base, XJobExecutor):
    def __init__(self, ctx):
        self.ctx = ctx

    def trigger(self, args):
        """Function calls from Addons.xcu"""
        if args == "insert":
            atomes_extension.insert_atomes_file()
        elif args == "open":
            atomes_extension.open_atomes_file()
        else:
            print(f"Commande inconnue : {args}")

g_ImplementationHelper = unohelper.ImplementationHelper()
g_ImplementationHelper.addImplementation(
    AtomesService,
    "fr.ipcms.atomes.AtomesService",
    ("com.sun.star.task.Job",)
)
