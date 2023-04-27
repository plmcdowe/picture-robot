import builtins
import serial
import tkinter as tk
from tkinter import font
import time

LARGE_FONT= ("Helvetica", 18)
MED_FONT= ("Helvetica", 16)
SMALL_FONT= ("Helvetica", 12)
class robotApp(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        container = tk.Frame(self)
        self.arduinoData = arduinoData
        
        container.pack(side="top", fill="both", expand = True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, PageOne, PageTwo):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()


class StartPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = tk.Label(self, text="Is a reset required?", font=LARGE_FONT)
        label.pack(pady=10,padx=10)

        button = tk.Button(self, text="Yes", bg="#3399FF", fg="black",
                            activebackground="#0000CC", activeforeground="white",
                            borderwidth=4, font=MED_FONT,
                            command=lambda:controller.show_frame(PageOne))
        button.pack(pady=5)

        button2 = tk.Button(self, text="No", bg="#3399FF", fg="black",
                            activebackground="#0000CC", activeforeground="white",
                            borderwidth=4, font=MED_FONT,
                            command=lambda: controller.show_frame(PageTwo))
        button2.pack(pady=5)

class PageOne(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.arduinoData = arduinoData
        label = tk.Label(self, text="Choose a reset option:", font=LARGE_FONT)
        label.pack(pady=10,padx=10)

        self.buttonFr = tk.Button(self, text="Reset arm & body", bg="#3399FF", fg="black",
                            activebackground="#0000CC", activeforeground="white",
                            borderwidth=4, font=MED_FONT,
                            command=lambda:[self.full_reset(), self.ser_data()])
        self.buttonFr.pack(pady=5)

        self.buttonAr = tk.Button(self, text="Reset only arm", bg="#3399FF", fg="black",
                            activebackground="#0000CC", activeforeground="white",
                            borderwidth=4, font=MED_FONT,
                            command=lambda:[self.lift_level(), self.ser_data()])
        self.buttonAr.pack(pady=5)

        self.buttonNxt = tk.Button(self, text="Next", bg="#e6e600", fg="black",
                            activebackground="#808080", activeforeground="white",
                            borderwidth=4, font=MED_FONT, state=tk.DISABLED,
                            command=lambda:[self.button_reset(), controller.show_frame(PageTwo)])
        self.buttonNxt.pack(pady=5)
        
    def full_reset(self):
        self.buttonAr['state'] = 'disabled'
        self.buttonFr['state'] = 'disabled'
        self.fullReset = bytes('f>', encoding='utf8')
        self.arduinoData.write(self.fullReset)

    def lift_level(self):
        self.buttonAr['state'] = 'disabled'
        self.buttonFr['state'] = 'disabled'
        self.armReset = bytes('a>', encoding='utf8')
        self.arduinoData.write(self.armReset)

    def ser_data(self):
        while True: 
            ser = arduinoData.readline().rstrip(b'r\r\n ').decode()
            
            if ser == '':
                continue
            elif ser == 'k':
                self.buttonNxt['state'] = 'normal'
                self.buttonAr['state'] = 'disabled'
                self.buttonFr['state'] = 'disabled'
                break

    def button_reset(self):
        self.buttonAr['state'] = 'normal'
        self.buttonFr['state'] = 'normal'
        self.buttonNxt['state'] = 'disabled'

class PageTwo(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)

        self.var = tk.StringVar()
        
        label = tk.Label(self, text="Enter No. of Iterations:", font=LARGE_FONT)
        label.pack(pady=10,padx=10)

        self.Entry = tk.Entry(self, font=SMALL_FONT)
        self.Entry.pack()

        self.buttonCheck = tk.Button(self, text="Check", bg="#FFFF00", fg="black",
                            activebackground="#999900", activeforeground="white",
                            borderwidth=4, font=MED_FONT,
                            command=lambda: self.check_entry())
        self.buttonCheck.pack()

        self.buttonStart = tk.Button(self, text="Start", bg="#00CC00", fg="black",
                            activebackground="green", activeforeground="white",
                            borderwidth=4, font=MED_FONT, state=tk.DISABLED,
                            command=lambda: [self.start_machine(), self.ser_data()])
        self.buttonStart.pack()

        self.buttonReset = tk.Button(self, text="Return to Reset Options", bg="#3399FF", fg="black",
                            activebackground="#0000CC", activeforeground="white",
                            borderwidth=4, font=SMALL_FONT,
                            command=lambda: controller.show_frame(PageOne))
        self.buttonReset.pack(pady=5)

        self.statusBar = tk.Label(self, textvariable=self.var, bd=4, font=SMALL_FONT, bg="white", relief=tk.SUNKEN, anchor=tk.W)
        self.statusBar.pack(side=tk.BOTTOM, fill=tk.X, expand=True)


    def check_entry(self):
        userEntry = self.Entry.get()
        
        if userEntry.isalpha() == False:
            
            if float(userEntry).is_integer() == True:
                val = int(userEntry)
                if val >= 1 and val <= 15:
                    self.buttonStart['state'] = 'normal'
                    self.buttonCheck['state'] = 'disabled'

                else:
                    self.Entry.delete(0, tk.END)
                    self.Entry.insert(0, "non-float >1 & <=15")
            else:
                self.Entry.delete(0, tk.END)
                self.Entry.insert(0, "non-float >1 & <=15")

        else:
            self.Entry.delete(0, tk.END)
            self.Entry.insert(0, "enter a number")

    def start_machine(self):
        userEntry = self.Entry.get()
        val = int(userEntry)
        endMarker = bytes('>', encoding='utf8')
        byteU = bytes(userEntry, encoding='utf8')
        data = b"".join([byteU, endMarker])
        arduinoData.write(data)
        self.buttonStart['state'] = 'disabled'
        self.buttonReset['state'] = 'disabled'

    def ser_data(self):
        status = 0
        userEntry = self.Entry.get()
        while True:
            ser = arduinoData.readline().rstrip(b'r\r\n ').decode()

            if ser == '':
                continue
            elif ser == '0':
                self.var.set("complete!")
                self.update()
                self.Entry.delete(0, tk.END)
                self.buttonCheck['state'] = 'normal'
                self.buttonReset['state'] = 'normal'
                break
            else:
                status = "on number " + str(ser) + " of " + userEntry
                self.var.set(status)
                self.update()
      
arduinoData = serial.Serial('COM4', 9600, timeout=0, writeTimeout=0)
app = robotApp()   
app.mainloop()


