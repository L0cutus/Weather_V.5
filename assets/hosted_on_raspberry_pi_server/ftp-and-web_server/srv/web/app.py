import io
from flask import Flask, send_file

app = Flask(__name__)

@app.route('/<pth>/<file>', methods=['GET', 'POST'])
def firmware(pth, file):
  """Serves the Weather Station V.5 firmware"""
  print(pth + '/' + file)
  #with open("/firmware/" + file, 'rb') as bites:
  return send_file(pth + '/' + file, mimetype='application/octet-stream' )

