FROM nginx

VOLUME [ "/wks", "/src" ]
WORKDIR /src/nginx

RUN apt -y update && \
    apt -y upgrade && \
    apt -y install gcc g++ make git

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["nginx", "-g", "daemon off;"]
