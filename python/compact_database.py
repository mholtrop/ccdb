#!/usr/bin/python3

run = 30300
runMin = 30000
runMax = 30100
calibtime = '2019-07-04'
variation = 'mcc_cpp'

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
    print('dumping ccdb database')
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

def count_rows(table):
    print('counting rows in', table)
    count_rows_in_assignments = 'select count(*) from ' + table + ';'
    mycursor.execute(count_rows_in_assignments)
    myresult = mycursor.fetchall()
    for x in myresult:
        print('info: row count = ', x)


count_rows('assignments')
        
delete_other_runs = 'delete from assignments where id in (select * from (select a.id from assignments as a, runRanges as r where runRangeId = r.id and (runMin > 31000 or runMax < 31000))tmptable);'
mycursor.execute(delete_other_runs)

count_rows('assignments')

count_rows('constantSets')

delete_unused_constant_sets = 'delete from constantSets where id not in (select constantSetId from assignments);'
mycursor.execute(delete_unused_constant_sets)

count_rows('constantSets')

mydb.commit()

exit

get_target_varId = 'select id from variations where name = "mc_cpp";'
mycursor.execute(get_target_varId)
myresult = mycursor.fetchall()
varId = myresult[0][0]
print(type(varId), 'varId =', varId)
varId_list=[]
varId_list.append(varId)
get_parentId = 'select parentId from variations where id = ' + str(varId) + ';'
mycursor.execute(get_parentId)
myresult = mycursor.fetchall()
parentId = myresult[0][0]
print('parentId of target =', parentId)

while parentId != 0: # 
    varId_list.append(parentId)
    get_next_parentId = 'select parentId from variations where id = ' + str(parentId) + ';'
    mycursor.execute(get_next_parentId)
    myresult = mycursor.fetchall()
    parentId = myresult[0][0]
    print('next parentId =', parentId)

print('list of ids =', varId_list)
or_clause = ''
for id in varId_list:
  if id == varId:
    or_word = ''
  else:
      or_word = ' OR '
  or_clause += or_word + 'variationId = ' + str(id)

print('OR clause =', or_clause)


delete_assignments_newer_than_timestamp = 'delete from assignments where created > "2019-07-04";'

delete_unused_run_ranges = 'delete from runRanges id not in (select runRangeId from assignments);'

mycon.close()

os.system('$CCDB_HOME/scripts/mysql2sqlite/mysql2sqlite.sh -hlocalhost -uccdb_user ccdb_marki | sqlite3 ccdb_marki.sqlite')
exit
