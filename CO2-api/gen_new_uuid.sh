#!/bin/bash -e

MODULE_TOKEN_FN="module_token.h"
MAPPING_FN="mapping.json"

echo -e "Dieses Skript\n\
- generiert eine neue UUID\n\
- frägt nach einer menschenlesbaren Bezeichnung für\n\
  das Modul wie z.B. der Raumname, der später in Grafana verwendet wird.\n\
  Nur die ersten 8 Zeichen werden auf dem OLED Display dargestellt.\n\
- Trägt die neue UUID und den Bezeichner in \"$MODULE_TOKEN_FN\"\n\
  und \"$MAPPING_FN\" ein\n\
- Fordert zum Programmieren des Moduls via Arduino IDE auf\n"

UUID=$(uuidgen)

echo -n "Bitte den Bezeichner (z.B. Raumname) eingeben: "
read IDENT

echo "const char *module_token = \"${UUID}\";" > "$MODULE_TOKEN_FN"
echo "const char *module_id = \"${IDENT}\";" >> "$MODULE_TOKEN_FN"

if [ ! -f "$MAPPING_FN" ]; then
  echo "[]" > "$MAPPING_FN"
fi

# Ich will nicht sponge vorraussetzen, daher umweg über tempfile
TMP=$(tempfile)
jq --arg token "$UUID" --arg id "$IDENT" '. += [{"token":$token, "id":$id}]' "$MAPPING_FN" > "$TMP" || exit -10
mv -f "$TMP" "$MAPPING_FN"

echo -e "\nDas Modul kann jetzt über die Arduino IDE über\n\
\"Sketch->Hochladen\" programmiert werden."

# Debugging:
#cat "$MODULE_TOKEN_FN"
#cat "$ROOM_MAPPING_FN"
