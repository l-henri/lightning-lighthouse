#!/usr/bin/python
import serial
import time
import email
import getpass, imaplib
import json

#The following line is for serial over GPIO
# Macbook
port = 'COM9'
# Raspberry
# port = '/dev/ttyUSB0'

emailCredentialsJson = 'emailCredentials.json'

def initArduino():
	ard = serial.Serial(port,9600,timeout=1)
	time.sleep(5)
	return ard

def sendMessageToArduino(message):

	print(ard.is_open)
	ard.write(message)
	time.sleep(1)
	print(ard.out_waiting)
	readMessage = ard.readline()
	utf8Message = readMessage.decode("utf-8")
	while (utf8Message[0:2] != "OK"):
	    time.sleep(1)
	    readMessage = ard.readline()
	    utf8Message = readMessage.decode("utf-8")

def loadEmailCredentials():
	with open(emailCredentialsJson) as emailCredentialsJsonLoaded:
		retrievedElements = json.load(emailCredentialsJsonLoaded)
		emailCredentialsJsonLoaded.close()
		return retrievedElements

def checkEmail(emailCredentials):
	userName = emailCredentials["email"]
	passwd = emailCredentials["password"]
	emailServer = emailCredentials["emailServer"]
	# try:
	print("Connecting to imap server")
	imapSession = imaplib.IMAP4_SSL(emailServer)
	print("Opening session")
	typ, accountDetails = imapSession.login(userName, passwd)
	if typ != 'OK':
		print('Not able to sign in!')
		raise
	print("Selecting emails")
	imapSession.select('INBOX')
	print("Searching")
	typ, data = imapSession.search(None, 'ALL')
	if typ != 'OK':
		print('Error searching Inbox.')
		raise
	print("Iterating over all emails")
	# Iterating over all emails
	print(data)
	for element in data:
		print(data[element])
	for msgId in data[0].split():
		typ, messageParts = imapSession.fetch(msgId, '(RFC822)')
		if typ != 'OK':
			print('Error fetching mail.')
			raise

		emailBody = messageParts[0][1]
		mail = email.message_from_string(emailBody)
		print(mail)
	imapSession.close()
	imapSession.logout()
	# except :
	#     print('Not able to download all attachments.')

def run():
	_emailCredentials = loadEmailCredentials()
	checkEmail(_emailCredentials)
	_message = 'sos'.encode("utf-8")
	# _ardModule = initArduino()
	# sendMessageToArduino(_message)

if __name__ == '__main__':
  run()