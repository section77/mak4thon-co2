#!/bin/bash -e

OUT_FN="auto_dashboard.json"
cp dashboard_template.json "$OUT_FN"

nr=0;
jq '.[] | .id' mapping.json | while read l; do
  TMP="hehe";#$(tempfile)
  jq ".targets[0].tags[0].value = ${l} | .title=${l} | .id=1+${nr} | .gridPos.y=9*${nr}" panel_template.json > "$TMP"
  jq --slurpfile panel "$TMP" '.panels += $panel' "$OUT_FN" | sponge "$OUT_FN"
  nr=$((nr+1))
done

cp "$OUT_FN" ../grafana/dashboards
