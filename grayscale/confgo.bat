@echo off

docker compose down grayscale-nginx --rmi local

docker compose up grayscale-nginx -d

docker exec -it grayscale-nginx /confgo