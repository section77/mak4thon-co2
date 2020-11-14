FROM php:7.2-fpm

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    supervisor

COPY supervisord.conf /etc/supervisor
COPY ./config/supervisor/* /etc/supervisor/conf.d
COPY php /var/www/php

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/supervisord.conf"]