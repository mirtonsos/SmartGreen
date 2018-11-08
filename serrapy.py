import serial
from collections import OrderedDict
import telegram
from telegram import KeyboardButton as BT
from telegram.ext import Updater, CommandHandler, MessageHandler, Filters

import logging

import pygame
import pygame.camera

pygame.camera.init()
camlist = pygame.camera.list_cameras()
print(camlist)
cam = pygame.camera.Camera("/dev/video0",(800,600))
cam.start()

def getpicture(bot,update):
    print("Get image")
    image = cam.get_image()
    image = cam.get_image()
    image = cam.get_image()
    pygame.image.save(image, "po.jpg")
    bot.send_photo(chat_id=update.message.chat_id,photo=open("po.jpg",'rb'))


# Enable logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                    level=logging.INFO)

logger = logging.getLogger(__name__)


valori = {}

def info(bot, update):
    tastiera = telegram.ReplyKeyboardMarkup([
        [BT("info"), BT("picture")],
        ])
    global valori
    try:
        update.message.reply_text(
        "\n".join(["%s: %s" % (key.title(), str(value)) for key, value in valori.items()]),
        reply_markup=tastiera
        )
    except Exception as e:
        print(str(e))
        
        
def settings(bot, update):
    if update.message.text == "info": info(bot, update)
    if update.message.text == "picture": getpicture(bot, update)
        
        

updater = Updater("id bot")
dp = updater.dispatcher
dp.add_handler(MessageHandler(Filters.text, settings))
dp.add_handler(CommandHandler("info", info))
dp.add_handler(CommandHandler("picture", getpicture))
updater.start_polling()

with serial.Serial('/dev/ttyUSB1', 19200) as ser: 
	while 1:
		inp = ser.readline().decode().split(",")
		print(inp)
		if not len(inp) == 17: continue
		valori = OrderedDict((
                    ("luce", "accesa" if inp[1]=="1" else "spenta"),
                    ("luce accesa", inp[2]+" ore"),
                    ("luce spenta", inp[3]+" ore"),
                    
                    ("ventola", "accesa" if inp[14]=="1" else "spenta"),
                    ("ventola spenta", inp[9]+" minuti"),
                    ("ventola accesa", inp[10]+" minuti"),
                    
                    ("pompa", "accesa" if inp[6]=="1" else "spenta"),
                    ("pompa accesa", inp[7]+" minuti"),
                    ("pompa spenta", inp[8]+" minuti"),
                    
                    ("CO2", inp[4]),
                    ("secco", "no" if int(inp[5])<90 else "si"),
                    ("massima CO2", inp[12]),
                    ("massima temperatura", str(int(inp[11])/10)+"°"),
                    ("umidita", inp[13]+"%"),
                    ("temperatura", str(int(inp[15])/10)+"°"),
                    ))
		print(valori)

