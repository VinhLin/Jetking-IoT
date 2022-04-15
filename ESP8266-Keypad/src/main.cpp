#include <Arduino.h>

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const byte n_rows = 4;
const byte n_cols = 4;

#define Password_Length 8

int signalPin = 12;
int signalPin_Speaker = 62;

String pass_true = "123A456B";
String key_word;

char keys[n_rows][n_cols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// byte colPins[n_rows] = {D3, D2, D1, D0};
// byte rowPins[n_cols] = {D7, D6, D5, D4};

// Arduino
byte colPins[n_rows] = {9, 8, 7, 6};
byte rowPins[n_cols] = {5, 4, 3, 2};

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, n_cols, n_rows);
// Keypad myKeypad = Keypad(makeKeymap(keys), colPins, rowPins, n_cols, n_rows);

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool checkPassword(String keypass);

void setup()
{
  Serial.begin(9600);

  pinMode(signalPin, OUTPUT);
  pinMode(signalPin_Speaker, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void loop()
{
  char myKey = myKeypad.getKey();

  if (myKey == '#')
  {
    changePassword();
  }

  lcd.setCursor(0, 0);
  lcd.print("Enter Password: ");

  if (myKey != NULL)
  {
    key_word += String(myKey);

    lcd.setCursor(0, 1);
    lcd.print(key_word);

    if (key_word.length() == 8)
    {
      if (checkPassword(key_word))
      {
        // active I/O
        digitalWrite(signalPin, HIGH);

        delay(3000);

        digitalWrite(signalPin, LOW);
      }
      else
      {
        // wrong password. ON speaker
        tone(signalPin_Speaker, 2500, 200);

        // analogWrite(signalPin_Speaker, 191);
        // delay(5000);
        // analogWrite(signalPin_Speaker, 0);
      }
    }

    // clear string
    key_word = "";
  }
}

void changePassword()
{
  String key_pass;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Old Password");

  while (1)
  {
    char myKey = myKeypad.getKey();

    if (myKey)
    {
      key_pass += String(myKey);
      if (key_pass.length() == 8)
      {
        if (checkPassword(key_pass))
        {
          lcd.clear();
          lcd.print("Change success");
          delay(2000);

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("New Pass:");
          
        }
      }
    }
  }
}

bool checkPassword(String keypass)
{
  if (keypass == pass_true)
  {
    Serial.print("Pass Access: ");
    Serial.println(keypass);

    lcd.clear();
    lcd.print("Pass Access");

    return true;
  }

  lcd.clear();
  lcd.print("Wrong Password");

  return false;
}