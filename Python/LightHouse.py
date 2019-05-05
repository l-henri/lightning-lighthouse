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
      return (retrievedElements['email'], retrievedElements['password'])

def run():

	_message = 'sos'.encode("utf-8")
	sendMessageToArduino(_message)

def checkEmail():
	userName = ''
	passwd = ''
	try:
	    print "Connecting to imap server"
	    imapSession = imaplib.IMAP4_SSL('mail.register.eu')
	    print "Opening session"
	    typ, accountDetails = imapSession.login(userName, passwd)
	    if typ != 'OK':
	        print 'Not able to sign in!'
	        raise
	    print "Selecting emails"
	    imapSession.select('INBOX')
	    # typ, data = imapSession.search(None, 'ALL')
	    deadline = time.time() - picDisplayTime
	    requestTime = time.strftime('%d-%b-%Y', time.localtime(deadline))
	    print "Searching inbox for emails after " + str(requestTime)

	    typ, data = imapSession.search(None, '(SINCE ' + requestTime+ ')')
	    
	    if typ != 'OK':
	        print 'Error searching Inbox.'
	        raise
	    print "Iterating over all emails"
	    # Iterating over all emails
	    for msgId in data[0].split():
	        typ, messageParts = imapSession.fetch(msgId, '(RFC822)')
	        if typ != 'OK':
	            print 'Error fetching mail.'
	            raise

	        emailBody = messageParts[0][1]
	        mail = email.message_from_string(emailBody)
	        for part in mail.walk():

	            if part.get_content_maintype() == 'multipart':
	                # print part.as_string()
	                continue
	            if part.get('Content-Disposition') is None:
	                # print part.as_string()
	                continue
	            fileName = part.get_filename()
	            # Checking if a file is attached
	            if bool(fileName):
	                filePath = os.path.join(detach_dir, 'pics', fileName)
	                if not os.path.isfile(filePath) :
	                    print "File %s has been found" % fileName
	                    # print filePath
	                    fp = open(filePath, 'wb')
	                    fp.write(part.get_payload(decode=True))
	                    fp.close()
	                    checkOrientation(filePath)
	    imapSession.close()
	    imapSession.logout()
	except :
	    print 'Not able to download all attachments.'


if __name__ == '__main__':
  run()