from pydoc import describe
import discord, serial
from discord.ext import commands
client = commands.Bot(command_prefix="!")
serport = serial.Serial("COM3", baudrate=9600, timeout=1)

@client.event
async def on_ready():
    print("Bot Prepared!")

@client.command(name="getTemp", brief="Returns currently temperature data", description="Returns currently temperature data")
async def getTemp(ctx):
    serport.write(b'g\n')
    stripped_temp = ""
    while not isfloat(stripped_temp):
        try:
            line = serport.readline()
            temp = line.decode()
            stripped_temp = temp.strip()
        except:
            pass
    line = serport.readline()
    temp = line.decode()
    Unit = temp.strip()
    await ctx.send("อุณหภูมิ: {} °{}".format(stripped_temp, Unit))

@client.command(name="getLimit", brief="Returns limit temperature", description="Return max temperature value that you want to be warn")
async def getLimit(ctx):
    serport.write(b'l\n')
    stripped_temp = ""
    while not isfloat(stripped_temp):
        try:
            line = serport.readline()
            temp = line.decode()
            stripped_temp = temp.strip()
        except:
            pass
    await ctx.send("อุณหภูมิที่กำหนด: " + stripped_temp + " °C")

@client.command(name="setLimit", brief="Set limit temperature", description="Setting max temperature value that you want to be warn")
async def setLimit(ctx, arg = None):
    if arg is not None and isfloat(arg):
        num = str(arg) + "\n"
        byte_num = num.encode()
        serport.write(b's\n')
        serport.write(byte_num)
        await ctx.send("ตั้งค่าอุณหภูมิสูงสุดเป็น " + num.strip() + " °C")
    else:
        await ctx.send("โปรดระบุอุณหภูมิให้ถูกต้อง")

@client.command(name="setUnit", brief="Set temperature unit", description="Set temperature unit that will be show")
async def setUnit(ctx, arg = None):
    if arg is not None and isUnit(arg):
        unit = str(arg) + "\n"
        byte_unit = unit.encode()
        serport.write(b'u\n')
        serport.write(byte_unit)
        await ctx.send("ตั้งค่าหน่วยของอุณหภูมิเป็น °" + unit.strip().upper())
    else:
        await ctx.send("โปรดระบุหน่วยให้ถูกต้อง")

def isfloat(num):
    try:
        float(num)
        return True
    except ValueError:
        return False

def isUnit(unit):
    unit = unit.upper()
    if unit == 'C' or unit == 'K' or unit == 'R' or unit == 'F':
        return True
    else:
        return False

# Run client ------------------------------------------------------------------- #
client.run("Bot_token")