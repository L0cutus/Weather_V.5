# Files located on Raspberry PI ftp and web server 

### Flask webserver
files on /home/pi/srv/web/

app.py
firmware/   # Firmware directory (were downloaded by board via web)
.venv

### FTP server
files on /home/pi/srv/ftp/

files/              # Where the board store the files to 
                    # be elaborated then by node-red:
                    # ATMData.json
                    # Data_Rdy.txt
                    # PMSData.json
                    # WINDData.json

files/firmware/fw-changelog.log     # Contains firmware update log

### Database SQLite
files on /home/pi/sql/

wind0.db            # Created by node-red sqlite engine
