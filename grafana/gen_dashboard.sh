#!/bin/bash -e


MAPPING_FN="../CO2-api/mapping.json"      # von gen_new_uuid.sh erzeugte Liste mit token und ids
OUT_FN="./dashboards/auto_dashboard.json" # die von diesem Skript erzeugte Datei, die in grafana importiert werden kann

cp dashboard_template.json "$OUT_FN"

nr=0;
jq '.[] | .id' "$MAPPING_FN" | while read l; do
  TMP=$(tempfile)
  gridPos_w=12;
  gridPos_h=9;
  gridPos_x=$((nr % 2 * $gridPos_w));
  gridPos_y=$((nr / 2 * $gridPos_h));
  echo "processing id ${l}..."
  echo $gridPos_w $gridPos_h $gridPos_x $gridPos_y
  jq ".targets[0].tags[0].value = ${l} | .title=${l} | .id=1+${nr} | .gridPos.w=${gridPos_w} | .gridPos.h=${gridPos_h}  | .gridPos.x=${gridPos_x} | .gridPos.y=${gridPos_y}" panel_template.json > "$TMP"
  jq --slurpfile panel "$TMP" '.panels += $panel' "$OUT_FN" | sponge "$OUT_FN"
  nr=$((nr+1))
done

echo "generated ${OUT_FN}..."
