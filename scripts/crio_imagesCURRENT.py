"""
This recipe describes how to handle asynchronous I/O in an environment where
you are running Tkinter as the graphical user interface. Tkinter is safe
to use as long as all the graphics commands are handled in a single thread.
Since it is more efficient to make I/O channels to block and wait for something
to happen rather than poll at regular intervals, we want I/O to be handled
in separate threads. These can communicate in a threasafe way with the main,
GUI-oriented process through one or several queues. In this solution the GUI
still has to make a poll at a reasonable interval, to check if there is
something in the queue that needs processing. Other solutions are possible,
but they add a lot of complexity to the application.

Created by Jacob Hallen, AB Strakt, Sweden. 2001-10-17
"""

"""
166 Edits:

- Recording outputs timestamp onto files
- Max number of images to be kept while recording, to be set by user as keepCount
- Images output to ./directory_of_script/output/

"""





import os
import Tkinter
from Tkinter import *
from PIL import Image,ImageTk
import StringIO
import time
import threading
import Queue
import time
import sys, os
import ftplib
import subprocess
import glob
import shutil


# Edit these:

#########################
hostname = "10.1.66.2"                # Host IP to connect to
username = "anonymous"                # FTP username (often times anonymous)
password = "guest"                    # FTP password (usually can be anything)
remoteDir = "/"                       # Directory where the image(s) is/are
remoteFile = "hslImage.jpg"          # Name of file you wish to download
keepCount = 60                       # Max number of files to be saved when recording images (edit as needed)
#########################
 
# Don't edit these:

#########################
recordText="Recording:Press_to_Stop"  # These buttons currently don't accept spaces,
playText  ="Playing:Press_to_Record"  # and are part of the GUI so don't change them.
#########################

class ManagedGUI(object):
    def __init__(self, master, queue, imageFilename, endCommand):
        self.master = master
        self.queue = queue
        self.imageFilename = imageFilename
        self.titleText = "RobotI" + " " + remoteFile
        self.time = 0
        self.toggle = 0
        self.record = False
        self.cwd = os.getcwd()

        self.master.title(self.titleText)

        # Initialize GUI components

        self.frame = Tkinter.Frame(self.master)
        self.modeButton = Tkinter.Button(self.frame,
            text=playText, command=self.modeCommand)
        self.modeButton.pack(side='left')
        self.quitButton = Tkinter.Button(self.frame,
            text='Quit (NOT-CONNECTED)', command=endCommand)
        self.quitButton.pack(side='left')
        self.frame.pack()
        self.fcanvas = Tkinter.Frame(self.master)
        self.canvas = Tkinter.Canvas(self.fcanvas)
        self.canvas.pack(expand=YES,fill=BOTH)
        self.fcanvas.pack(expand=YES,fill=BOTH)

        self.master.protocol ("WM_DELETE_WINDOW",endCommand)

    def modeCommand(self):
      if self.modeButton.configure('text')[4] == playText:
          self.modeButton.configure(text=recordText)
          self.record = True
      else:
          self.modeButton.configure(text=playText)
          self.record = False



    def processQueue(self):
        """
        Handle all the messages currently in the queue
        """
        while self.queue.qsize():
            try:
                msg = self.queue.get(0)

                if msg == 'imageFile':
                  self.time = time.time()
                  # Set the image TIME
                  self.master.title(self.titleText+"  "+str(time.ctime()))

                  self.imageFile = os.path.join(self.cwd, self.imageFilename.get(0))

                  #continue

                  self.imageRaw = Image.open(self.imageFile)

                  self.geometry=self.master.geometry()
                  self.x=self.geometry.split('+')[0].split('x')[0]
                  self.y=self.geometry.split('+')[0].split('x')[1]
                  self.imageRaw.thumbnail((int(self.x),int(self.y)))
                  self.image = ImageTk.PhotoImage(self.imageRaw)
                  self.canvas.create_image(0,0,image=self.image,anchor='nw')

                  if self.record:
                    newFile = os.path.splitext(self.imageFile)
                    self.recordFile = (newFile[0] + '-' + str(int(time.time())) + ".jpg")
                    shutil.copy(self.imageFile, self.recordFile)
                    print "Recorded Image File:%s" %(self.recordFile)
                    searchString = newFile[0] + '*.jpg'
                    f = glob.glob(searchString)
                    if len(f) > keepCount:
                        f = sorted(f)
                        os.remove(os.path.join(self.cwd, f[0]))

                elif msg == 'main':
                  if (self.time + 10) > time.time():
                    if self.toggle:
                      self.toggle=0
                      self.modeButton.configure(bg="green")
                    else:
                      self.toggle=1
                      self.modeButton.configure(bg="white")
                  else:
                    self.modeButton.configure(bg="red")
                  pass

                elif msg == 'connected':
                  self.quitButton.configure(text="Quit (CONNECTED)")

                else:
                  pass

            except Queue.Empty:
                pass

class ThreadedGUI(object):
    """
    Launch the main part of the GUI and the worker thread.
    """
    def __init__(self, master):
        """
        Start the GUI and the asynchronous threads. We are in the main
        thread of the application, which will later be used by
        the GUI. We spawn new threads for the workers.
        """
        self.master = master

        # Create the queues
        self.queue = Queue.Queue()
        self.imageData = Queue.Queue()

        # Set up the GUI part
        self.gui = ManagedGUI(master,
          self.queue, self.imageData, self.endApplication)

        # Set up the threaded part
        self.running = 1
        self.thdMain = threading.Thread(target=self.wthdMain)
        self.thdMain.setDaemon(True)
        self.thdMain.start()
        self.thdImage = threading.Thread(target=self.wthdImage)
        self.thdImage.setDaemon(True)
        self.thdImage.start()

        # Start the periodic call in the GUI to check if the
        # queue contains anything
        self.wthdPeriodic()

    def endApplication(self):
        self.running = 0

    def wthdPeriodic(self):
        """
        Check every 250 ms if there is something new in the queue.
        """
        if not self.running:
            # This is the brutal stop of the system.
            import sys
            sys.exit(1)
        self.gui.processQueue()
        self.master.after(250, self.wthdPeriodic)

    def wthdMain(self):
        """
        This is where we handle the asynchronous I/O.
        """
        while self.running:
            time.sleep(1.0)
            self.queue.put("main")

    def wthdImage(self):
        """
        This thread captures and forwards an imageFile for processing
        """
        self.connect = None
        self.ftp = None
        while self.running:
            time.sleep (1)
            if not self.connect: # We need to connect (ping) to the image server
                print "Attempting system ping the image server:", hostname
                try:
                    value = os.system("ping " + hostname)
                    if value == 0:
                      print "Connected to image server:", hostname
                      self.connect = True
                    else:
                      self.connect = False
                except:
                    print "Could not ping the image server:%s", hostname
                    self.connect = None
                    continue  # And try again if unsuccessful

            # Try to establish an FTP connection to the image server
            if self.connect and not self.ftp:
                try:
                    self.ftp = ftplib.FTP(hostname,username,password)
                    print "FTP established to image server:", hostname
                except Exception:
                    print "Unable to establish FTP with image server:%s user:%s password:%s" %(hostname,username,password)
                    continue
                try:
                    self.ftp.cwd(remoteDir)
                    self.queue.put("connected")

                except Exception:
                    print "Unable to FTP connect and CWD to: ",remoteDir
                    continue

            if self.connect and self.ftp: # Now we can do the work...
                try:
                    # Use the FTP connection to retrieve the file
                    self.gFile = open(remoteFile, "wb")
                    self.ftp.retrbinary('RETR '+remoteFile, self.gFile.write)
                    self.ftp.delete(remoteFile)
                    self.gFile.close()
                        
                    # Send file name to be processed
                    self.imageData.put(remoteFile)
                    self.queue.put("imageFile")

                except:
                    #print 'Could not process file from image server'
                    #self.ftp = None
                    #self.connect = None
                    pass

if __name__ == '__main__':
    root = Tkinter.Tk()
    gui = ThreadedGUI(root)
    root.mainloop()
