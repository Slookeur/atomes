#!/bin/sh

INSTALL_DIR=$1
prog_metadir=${INSTALL_DIR}/metainfo
prog_desktopdir=${INSTALL_DIR}/applications
prog_iconsdir=${INSTALL_DIR}/pixmaps

appstream-util validate-relax --nonet ${prog_metadir}/fr.ipcms.atomes.appdata.xml
desktop-file-install --vendor="" --dir=${prog_desktopdir} -m 644 ${prog_desktopdir}/atomes.desktop
touch --no-create ${prog_iconsdir}

if [ -u 'which gtk-update-icon-cache' ]; then 
  gtk-update-icon-cache -q ${prog_iconsdir}
fi
update-desktop-database ${prog_desktopdir} &> /dev/null || :; 
update-mime-database ${prog_datadir}/mime &> /dev/null || :; 
