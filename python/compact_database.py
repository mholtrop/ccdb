#!/usr/bin/python3

import os
import mysql.connector
import datetime

now = datetime.datetime.now()
dbserver = 'localhost'
dbuser = 'ccdb_user'
user = os.getenv('USER')
dbdumpfile = '/tmp/ccdb_dump_' + user + '_' + str(now.hour) + '.sql'
dbname = 'ccdb_' + user
dbpasswd = ""
print(dbserver, dbuser, dbdumpfile, dbname)

fileExists = os.path.exists(dbdumpfile)

if not fileExists:
    os.system('mysqldump -h ' + dbserver + ' -u ' + dbuser + ' ccdb > ' + dbdumpfile)

mydb = mysql.connector.connect(
  host=dbserver,
  user=dbuser,
  passwd=dbpasswd,
  database=dbname
)


mycursor = mydb.cursor()

print('drop database', dbname)
mycursor.execute("DROP DATABASE IF EXISTS " + dbname)
print('create database', dbname)
mycursor.execute("CREATE DATABASE " + dbname)
mycursor.execute("USE " + dbname)
print('load database from dumpfile', dbdumpfile)
os.system('mysql -h ' + dbserver + ' -u ' + dbuser + ' ' + dbname + ' < ' + dbdumpfile)
print('finished loading')
