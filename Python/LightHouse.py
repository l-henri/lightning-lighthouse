#!/usr/bin/python
import serial
import time
import email
import getpass, imaplib
import json


#The following line is for serial over GPIO
# Windows
# port = 'COM9'
# Raspberry
# port = '/dev/ttyUSB0'
# Macbook
port = '/dev/cu.wchusbserial1420'
emailCredentialsJson = 'emailCredentials.json'

def initArduino():
	ard = serial.Serial(port,9600,timeout=1)
	time.sleep(5)
	return ard

def sendMessageToArduino(message, ard):
	message = message.lower()
	message = message.encode("utf-8")
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
	authorizedEmailDomain = emailCredentials["authorizedEmailDomain"]
	authorizedSenders = emailCredentials["authorizedSenders"]
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
	typ, data = imapSession.search(None, 'NEW')
	if typ != 'OK':
		print('Error searching Inbox.')
		raise
	print("Iterating over all new emails")
	# Iterating over all emails
	# print(data)
	messagesList = []
	for msgId in data[0].split():
		typ, messageParts = imapSession.fetch(msgId, '(RFC822)')
		if typ != 'OK':
			print('Error fetching mail.')
			raise

		emailBody = messageParts[0][1].decode('utf-8')
		mail = email.message_from_string(emailBody)
		emailDomain = mail['from'].split('@')[1].split('>')[0]
		sender = mail['from'].split('<')[1].split('>')[0]
		if (emailDomain != authorizedEmailDomain):
			if sender not in authorizedSenders:
				continue
		# print(mail['subject'])
		messagesList.append(mail['subject'])


		# break
	imapSession.close()
	imapSession.logout()
	# except :
	#     print('Not able to download all attachments.')
	return messagesList

def run():
	_emailCredentials = loadEmailCredentials()
	_messagesList = checkEmail(_emailCredentials)
	print(_messagesList)
	if _messagesList == []:
		return
	_ardModule = initArduino()
	for _message in _messagesList:
		sendMessageToArduino(_message, _ardModule)

if __name__ == '__main__':
  run()