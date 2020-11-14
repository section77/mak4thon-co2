ARG GRAFANA_TAG
FROM grafana/$GRAFANA_TAG

ADD ./provisioning /etc/grafana/provisioning
ADD ./grafana.ini /etc/grafana/config.ini
ADD ./dashboards /var/lib/grafana/dashboards