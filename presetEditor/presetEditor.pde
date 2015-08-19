/**
 * ControlP5 Textfield
 *
 *
 * find a list of public methods available for the Textfield Controller
 * at the bottom of this sketch.
 *
 * by Andreas Schlegel, 2012
 * www.sojamo.de/libraries/controlp5
 *
 */

import processing.serial.*; //import the Serial library
import controlP5.*;
Serial myPort;  //the Serial port object
String val;
String send[] = new String[10];
String port;
int presetNr;
boolean firstContact = false;
ControlP5 cp5;
DropdownList d1, d2;
Textlabel l1p1;
Textlabel error1, error2, connect, notConnect;
Textfield t1p1, t2p1, t3p1, t4p1;
Textfield presetName;
String textValue = "";
PImage img;

void setup() {

  frameRate(24);
  size(900, 650);
  //img = loadImage("fondo2.jpg");

  cp5 = new ControlP5(this);
  drawEvents();
  drawPotis();
}  

void draw() {
  background(80);
  //image(img, 0, 0, 900, 625);
  drawManual();

  fill(40);
}

void drawPotis() {

  PFont font = createFont("arial", 12);
  int tmp = 1;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {

      int y = i*125;
      int x = j*180;

      l1p1 =  cp5.addTextlabel(tmp+".0")
        .setText("Poti " + tmp)
          .setPosition(60+x, 75+y)
            .setFont(createFont("arial", 18))
              ;

      t1p1 = cp5.addTextfield(tmp+".1")
        .setPosition(40+x, 100+y)
          .setSize(100, 15)
            .setFont(font)
              .setFocus(false)
                ;

      t2p1 = cp5.addTextfield(tmp+".2")
        .setPosition(40+x, 130+y)
          .setSize(45, 15)
            .setFont(font)
              .setFocus(false)
                ;

      t3p1 = cp5.addTextfield(tmp+".3")
        .setPosition(95+x, 130+y)
          .setSize(45, 15)
            .setFont(font)
              .setFocus(false)
                ;

      t4p1 = cp5.addTextfield(tmp+".4")
        .setPosition(40+x, 160+y)
          .setSize(100, 15)
            .setFont(font)
              .setFocus(false)
                ;    
      custom1(t1p1);
      custom2(t2p1);
      custom3(t3p1);
      custom4(t4p1); 

      tmp++;
    }
  }
}

void drawEvents() {

  PFont font = createFont("arial", 12);

  d1 = cp5.addDropdownList("myList-d1")
    .setPosition(40, 50)
      .setSize(180, 25)
        ;     
  customize1(d1);

  d2 = cp5.addDropdownList("myList-d2")
    .setPosition(400, 50)
      .setSize(75, 25)
        ;
  customize2(d2);

  presetName = cp5.addTextfield("PresetName")
    .setPosition(500, 33)
      .setSize(180, 15)
        .setFont(font)
          .setFocus(true)
            ;

  cp5.addBang("connect")
    .setPosition(240, 30)
      .setSize(80, 25)
        .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
          ; 

  cp5.addBang("send")
    .setPosition(600, 600)
      .setSize(80, 25)
        .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
          ;

  connect = cp5.addTextlabel("connected")
    .setText("Controller is connected")
      .setPosition(35, 600)
        .setFont(createFont("arial", 16))
          .setVisible(false)
            ;

  notConnect = cp5.addTextlabel("notConnected")
    .setText("Controller is not connected")
      .setPosition(35, 600)
        .setFont(createFont("arial", 16))
          .setVisible(true)
            ;

  error1 = cp5.addTextlabel("error1")
    .setText("please insert only a number from 0-127")
      .setPosition(300, 590)
        .setFont(createFont("arial", 16))
          .setVisible(false)
            ;

  error2 = cp5.addTextlabel("error2")
    .setText("for cc-Values and value range")
      .setPosition(300, 615)
        .setFont(createFont("arial", 16))
          .setVisible(false)
            ;
}

void drawManual() {
  fill(5, 53, 80);
  stroke(20, 106, 155);
  rect(720, 100, 150, 450);

  fill(255);
  String manual = "1. Select the port of the midicontroller\n\n2. Click 'CONNECT' (The midicontroller should restart and you see it at the bottom of the editor as well)\n\n3. Select the menu 'connect 2 editor' on the midicontroller\n\n4. Fill in all fields\n\n5. Click 'SEND'\n\n6. Wait until the midicontroller returns to the main screen";
  text(manual, 730, 110, 130, 440);
}

public void send() {
  send[0] = "";
  
  if (inputCorrect() == true) {
    //    send = createOutput();
    String tmp = createOutput();
    for ( int i = 0; i < 10; i++) {
      if ( i == 0) {
        send[i] = tmp.substring(0, 51);
      } else if ( i>0 && i<9 ) {
        send[i] = tmp.substring(i*50+1, i*50+51);
      } else {
        send[i] = tmp.substring(i*50+1);
      }
    }

    for (int i = 0; i < 10; i++) {

      delay(700);
      myPort.clear();
      myPort.write(send[i]);
      println(send[i]);
      delay(700);
    }
  } else {
    send[0] = "";
  }
  println(inputCorrect());
}

boolean inputCorrect() {

  boolean rtn = true;

  String presetName = cp5.get(Textfield.class, "PresetName").getText();
  if (presetName.length() > 16) {
    cp5.get(Textfield.class, "PresetName").setText(presetName.substring(0, 16));
  }

  for (int i = 0; i < 16; i++) {

    String potiName = cp5.get(Textfield.class, (i+1)+"."+1).getText();
    if (potiName.length() > 16) {
      cp5.get(Textfield.class, (i+1)+"."+1).setText(potiName.substring(0, 16));
    }

    String ccValue = cp5.get(Textfield.class, (i+1)+"."+4).getText();
    if (ccValue.length() != 0) {
      rtn = hasNoErrors(ccValue);
    }

    String ccMin = cp5.get(Textfield.class, (i+1)+"."+2).getText();
    if (ccMin.length() != 0) {
      rtn = hasNoErrors(ccMin);
    }

    String ccMax = cp5.get(Textfield.class, (i+1)+"."+3).getText();
    if (ccMax.length() != 0) {
      rtn = hasNoErrors(ccMax);
    }

    if ( ccMax.length() != 0 && ccMin.length() != 0 && isNumeric(ccMin) && isNumeric(ccMax)
    && Integer.parseInt(ccMin) > Integer.parseInt(ccMax) ) {
      cp5.get(Textfield.class, (i+1)+"."+3).setText(ccMin);
      cp5.get(Textfield.class, (i+1)+"."+2).setText(ccMax);
    }
}

  return rtn;
}

boolean hasNoErrors(String str) {

  boolean rtn = true;

  if (str.length() > 3 || isNumeric(str) == false ) 
  {
    rtn = false;
  } else if (Integer.parseInt(str) > 127 || Integer.parseInt(str) < 0)
  {
    rtn = false;
  } else
  {
    rtn = true;
  }
  setError(error1, error2, !rtn);
  return rtn;
}

boolean isNumeric(String toTest)
{
  for (int i=0; i < toTest.length (); i++)
  {  
    char c = toTest.charAt(i);
    if (c < '0' || c > '9')
    {
      return false;
    }
  }
  return true;
}



String createOutput() {

  String output;
  output = "$!"+presetNr;


  String tmp = cp5.get(Textfield.class, "PresetName").getText();
  String voids = "";

  for (int i=tmp.length (); i < 16; i++) {
    voids += " ";
  }
  output += "&"+tmp+voids;


  for (int i=0; i < 16; i++) {

    tmp = cp5.get(Textfield.class, (i+1)+"."+1).getText();
    voids = "";
    for (int j=tmp.length (); j < 16; j++) {
      voids += " ";
    }
    output += "&"+tmp+voids;
  }

  for (int i=0; i < 16; i++) {
    tmp = cp5.get(Textfield.class, (i+1)+"."+4).getText();
    String zero = "";
    for (int j=tmp.length (); j < 3; j++) {
      zero += "0";
    }
    output += "&"+zero+tmp;
  }

  for (int i=0; i < 16; i++) {
    tmp = cp5.get(Textfield.class, (i+1)+"."+2).getText();
    String zero = "";
    for (int j=tmp.length (); j < 3; j++) {
      zero += "0";
    }
    output += "&"+zero+tmp;
  }

  for (int i=0; i < 16; i++) {
    tmp = cp5.get(Textfield.class, (i+1)+"."+3).getText();
    String zero = "";
    for (int j=tmp.length (); j < 3; j++) {
      zero += "0";
    }
    output += "&"+zero+tmp;
  }

  return output+"&";
}



public void connect() {
  myPort = new Serial(this, port, 9600);
  myPort.bufferUntil('\n');
  delay(2000);
  setContact(connect, notConnect, true);

}

void controlEvent(ControlEvent theEvent) {
  if (theEvent.isAssignableFrom(Textfield.class)) {
    println("controlEvent: accessing a string from controller '"
      +theEvent.getName()+"': "
      +theEvent.getStringValue()
      );
  }

  if (theEvent.isGroup() && theEvent.name().equals("myList-d1")) {
    // check if the Event was triggered from a ControlGroup
    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
    port = Serial.list()[(int)theEvent.getGroup().getValue()];
    print(port);
  }

  if (theEvent.isGroup() && theEvent.name().equals("myList-d2")) {
    // check if the Event was triggered from a ControlGroup
    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
    presetNr = (int)theEvent.getGroup().getValue();
    println(presetNr);
  }
}



void serialEvent( Serial myPort) {
  //put the incoming data into a String - 
  //the '\n' is our end delimiter indicating the end of a complete packet
  val = myPort.readStringUntil('\n');
  //make sure our data isn't empty before continuing
  if (val != null) {
    //trim whitespace and formatting characters (like carriage return)
    val = trim(val);
    println(val);

    //look for our 'A' string to start the handshake
    //if it's there, clear the buffer, and send a request for data
    if (firstContact == false) {
      if (val.equals("A")) {
        myPort.clear();
        firstContact = true;
        myPort.write("A");
        println("contact");
        
      }
    } else { //if we've already established contact, keep getting and parsing data
      println(val);
      setContact(connect, notConnect, true);

    }
  }
}


void customize1(DropdownList ddl) {
  // a convenience function to customize a DropdownList
  ddl.setBackgroundColor(color(190));
  ddl.setItemHeight(20);
  ddl.setBarHeight(15);
  ddl.captionLabel().set("select port");
  ddl.captionLabel().style().marginTop = 3;
  ddl.captionLabel().style().marginLeft = 3;
  ddl.valueLabel().style().marginTop = 3;
  ddl.bringToFront();
  for (int i=0; i<Serial.list ().length; i++) {
    ddl.addItem(Serial.list()[i], i);
  }
  ddl.scroll(0);
  ddl.setColorBackground(color(60));
  ddl.setColorActive(color(255, 128));
}
void customize2(DropdownList ddl) {
  // a convenience function to customize a DropdownList
  ddl.setBackgroundColor(color(190));
  ddl.setItemHeight(20);
  ddl.setBarHeight(15);
  ddl.captionLabel().set("select preset");
  ddl.captionLabel().style().marginTop = 3;
  ddl.captionLabel().style().marginLeft = 3;
  ddl.valueLabel().style().marginTop = 3;
  ddl.bringToFront();
  for (int i=0; i<9; i++) {
    ddl.addItem(""+(i+1), i);
  }
  ddl.scroll(0);
  ddl.setColorBackground(color(60));
  ddl.setColorActive(color(255, 128));
}

void custom1(Textfield ddl) {
  ddl.captionLabel().set("name");
}
void custom2(Textfield ddl) {
  ddl.captionLabel().set("value range");
}
void custom3(Textfield ddl) {
  ddl.captionLabel().set("");
}
void custom4(Textfield ddl) {
  ddl.captionLabel().set("cc-value");
}
void setError(Textlabel e1, Textlabel e2, boolean b) {
  e1.setVisible(b);
  e2.setVisible(b);
}
void setContact(Textlabel con, Textlabel noCon, boolean b) {
  con.setVisible(b);
  noCon.setVisible(!b);
}


