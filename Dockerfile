from ubuntu

RUN apt-get update -y && apt-get install -y libcap-dev libcap2-bin
RUN useradd -u 1337 -d /app/server -s /usr/sbin/nologin app
COPY server /app/server
RUN setcap 'cap_net_bind_service=+ip' /app/server
WORKDIR /app/

USER app
CMD /app/server
