#include <SD.h>
#include <SPI.h>

const int chipSelect = 5;
File root;
File selFile;


// my first comment from my dad

//Commands ------------------
  String comList[] = {
    "help", //0 - help
    "ls", // 1 - rootls
    "write", // 2 - write [file name] [content]
    "mkdir", // 3 - mkdir [dir name]
    "rmdir", // 4 - rmdir [dir name]
    "rm", // 5 - remove [file name]
    "cat" //6 - fieldump
    ""
  };
//---------------------------
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }

  Serial.println("initialization done.");

  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("done!");
}

void loop() {
  if(Serial.available() > 0){
    String serialIn = Serial.readStringUntil('\n');
    exec(serialIn);
  }
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void exec(String command){
  String mainCommand = command.substring(0, command.indexOf(' '));
  String commandCtx1 = command.substring(command.indexOf(' ')+1, command.indexOf(' ', command.indexOf(' ')+1));
  String commandCtx2 = command.substring(command.indexOf(' ', command.indexOf(' ')+1)+1);
  
  //Debug innfo ----------------
  Serial.println("Debug info:");
  Serial.println(mainCommand);
  Serial.println(commandCtx1);
  Serial.println(commandCtx2);
  Serial.println(find(mainCommand));
  Serial.println("End of Debug info");
  
  //------------------------------ Command definition ------------------------------
  switch(find(mainCommand)){
    case -1:
      Serial.println("Error!! : Command not in command list (comList)");
      break;
    case 0:   //help
      Serial.println("Possible commannds are:");
      for(String comX : comList){
        Serial.println(comX);
      }
      break;
    case 1:   //rootls
      root = SD.open("/");
      Serial.println("Listing directory!:");
      printDirectory(root, 0);
      break;
    case 2:   //write
      if(commandCtx1.substring(commandCtx1.indexOf('.')) == ".txt"){
        if(SD.exists(commandCtx1)){
          Serial.println("writinng to existing file");
        }else{
          Serial.println("This file does not exist ... creatinng now one");
        }
        commandCtx1.trim();
        selFile = SD.open(commandCtx1, FILE_WRITE);
        selFile.seek(selFile.size());
        selFile.println(commandCtx2);
        selFile.close();
      }else{
        Serial.println("Error!! : Filetypes other than .txt are not allowed");
      }
      break;
    case 3:    //mkdir
      Serial.print("Creating directory : ");
      Serial.println(commandCtx1);
      if(SD.mkdir(commandCtx1)){
        Serial.print("Creation of : ");
        Serial.print(commandCtx1);
        Serial.println("   succesful!");
      }else{
        Serial.println("Directory creation failed!");
      }
      break;
    case 4:   //rmdir
      Serial.print("Removing directory : ");
      Serial.println(commandCtx1);
      if(SD.mkdir(commandCtx1)){
        Serial.print("Removal of : ");
        Serial.print(commandCtx1);
        Serial.println("   succesful!");
      }else{
        Serial.println("Directory removal failed!");
      }
      break;
    case 5:   //remove
      if(SD.exists(commandCtx1)){
        Serial.println("Deleting file...");
        Serial.print("Deletion ");
        if(SD.remove(commandCtx1)){
          Serial.println("successful");
        }else{
          Serial.println("failed!!");
        }
      }else{
        Serial.println("This file does not exist!!");
      }
      break;
    case 6:   //filedump
      selFile = SD.open(commandCtx1, FILE_READ);
      if (selFile) {
        Serial.println("File opened: " + commandCtx1);
        while (selFile.available()) {
          Serial.write(selFile.read());
        }
        selFile.close();
        Serial.println();
        Serial.println("File closed");
      } else {
        Serial.println("Error!! : File not correctly opened");
      }
      break; 
    default:   //default
      Serial.println("Error!! : Command found in (comList) but nnot definned in exec switch");
      break;
  }
}

int find(String findStr){
  for(int i = 0; i < sizeof(comList); i++){
    if(findStr == comList[i]){
      return i;
    }
  }
  return -1;
}