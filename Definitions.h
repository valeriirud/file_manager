#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DOMAIN_NAME "fm" 
#define APPLICATION_NAME "fm.app"
#define APPLICATION_TITLE "File Manager 0.1"

#define CMD_COPY "rsync -ah --progress \"%1\" \"%2\""
#define CMD_DELETE "rm -rfv \"%1\""
#define CMD_MOVE "mv \"%1\" \"%2\""
#define CMD_SIZE "du -h %1"
#define CMD_ZIP "zip -rdbj \"%1\" \"%2\""
#define CMD_UNZIP "unzip -d \"%1\" \"%2\""

#define ITEM_ICON 0
#define ITEM_NAME 1
#define ITEM_TYPE 2
#define ITEM_SIZE 3
#define ITEM_OWNER 4
#define ITEM_DATE 5
#define ITEMS_COUNT 6

#endif // DEFINITIONS_H
