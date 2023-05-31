#!/bin/bash

SERVERHOST="lica02.lab.unimo.it"
SERVERUSER="ftp"
SERVERPASS="esame"

# Initial checks
found=false
for desktop in Desktop Scrivania; do
  cd ~/$desktop/ >/dev/null 2>&1
  if [ "$?" -eq 0 ]; then
    found=true
    break
  fi
done

if [ "$found" == false ]; then
  echo "Cannot reach desktop. Check permissions."
  exit -1
fi

rm -rf .writetest
touch .writetest >/dev/null 2>&1
if [ "$?" -ne 0 ]; then
  echo "Cannot write on desktop. Check permissions."
  exit -1
fi

# Student interview
echo -n "Name and surname: "
read name
echo -n "Username (username@studenti.unimore.it): "
read student_id
echo -n "Turn ID: "
read turn_id
echo -n "Position: "
read position_id

# Padding turn and position with zeros
printf -v turn_id "%02d" ${turn_id#0}
printf -v position_id "%02d" ${position_id#0}

# Names definitions
FOLDERNAME=studente_"$turn_id"_"$position_id"_"$student_id"
FILENAME="$FOLDERNAME".tar.gz
IDFILENAME="student_data.csv"
PERIOD=60

# If folder does not exist, retrieve latest copy on server
if [ ! -d "$FOLDERNAME" ]; then

    curl ftp://"$SERVERHOST"/"$FILENAME" --user "$SERVERUSER":"$SERVERPASS" -o "$FILENAME" >/dev/null 2>&1
    if [ "$?" -eq 0 ]; then
      echo "Download [OK]"
      tar -xzf "$FILENAME" >/dev/null 2>&1
      rm "$FILENAME"
    else
      mkdir "$FOLDERNAME" >/dev/null 2>&1
    fi
    
fi

# Update latest copy on server
while true; do
  echo ""$name";"$student_id";"$USER";"$turn_id";"$position_id"" > "$FOLDERNAME"/"$IDFILENAME"

  tar -czf "$FILENAME" "$FOLDERNAME" >/dev/null 2>&1
  curl -T "$FILENAME" ftp://"$SERVERHOST" --user "$SERVERUSER":"$SERVERPASS" >/dev/null 2>&1
  if [ "$?" -eq 0 ]; then
    echo "Last Upload: `/bin/date` [OK]"
  else
    echo "Last Upload: `/bin/date` [FAILED]"
  fi
  rm -rf "$FILENAME"
  sleep "$PERIOD"
done
