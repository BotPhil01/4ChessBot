rm a.txt
rm b.txt
rm afiles.txt
rm names.txt

cd "./assets"
ls > "../afiles.txt"
cd ..

piece=""
declare -i row
declare -i col


# g() {
#     echo "in g"
#     echo "$piece"
#     if [[ "$piece" =~ [a-z]* ]]; then
#         echo "true"
#     else
#         echo "false"
#     fi
# }

# row and col calculation is for closest to a1 furthest from n14
# config for NESW = ygrb
calc_row() {
    if [[ "$piece" =~ r. ]]; then
        if [[ "$piece" =~ .p ]]; then
            row=2
        else
            row=1
        fi
    elif [[ "$piece" =~ y. ]]; then
        if [[ "$piece" =~ .p ]]; then
            row=13
        else
            row=14
        fi
    elif [[ "$piece" =~ b. ]]; then
        if [[ "$piece" =~ .[pr] ]]; then
            row=4
        elif [[ "$piece" =~ .n ]]; then
            row=5
        elif [[ "$piece" =~ .b ]]; then
            row=6
        elif [[ "$piece" =~ .q ]]; then
            row=7
        else
            row=8
        fi
    else
        if [[ "$piece" =~ .[pr] ]]; then
            row=4
        elif [[ "$piece" =~ .n ]]; then
            row=5
        elif [[ "$piece" =~ .b ]]; then
            row=6
        elif [[ "$piece" =~ .k ]]; then
            row=7
        else
            row=8
        fi
    fi
}

calc_col() {
    if [[ "$piece" =~ b. ]]; then
        if [[ "$piece" =~ .p ]]; then
            col=2
        else
            col=1
        fi
    elif [[ "$piece" =~ g. ]]; then
        if [[ "$piece" =~ .p ]]; then
            col=13
        else
            col=14
        fi
    elif [[ "$piece" =~ r. ]]; then
        if [[ "$piece" =~ .[pr] ]]; then
            col=4
        elif [[ "$piece" =~ .n ]]; then
            col=5
        elif [[ "$piece" =~ .b ]]; then
            col=6
        elif [[ "$piece" =~ .q ]]; then
            col=7
        else
            col=8
        fi
    else
        if [[ "$piece" =~ .[pr] ]]; then
            col=4
        elif [[ "$piece" =~ .n ]]; then
            col=5
        elif [[ "$piece" =~ .b ]]; then
            col=6
        elif [[ "$piece" =~ .k ]]; then
            col=7
        else
            col=8
        fi
    fi
}

f() {
    echo "running f on value: ${piece}"
    # red = d..k
    calc_col
    calc_row
    echo "${piece} col: ${col} row: ${row}"
    echo "<div class=\"piece-$piece\"></div>" >> "a.txt"
    # red and yellow
    if [[ "$piece" =~ [ry]p ]]; then
        for i in {1..7}; do
            col+=1
            echo "<div class=\"piece-$piece\"></div>" >> "a.txt"
        done
    elif [[ "$piece" =~ [ry]r ]]; then
        col+=7
    elif [[ "$piece" =~ [ry]n ]]; then
        col+=5
    elif [[ "$piece" =~ [ry]b ]]; then
        echo "here"
        col+=3
    elif [[ "$piece" =~ [bg]p ]]; then
        for i in {1..7}; do
            row+=1
            echo "<div class=\"piece-$piece\"></div>" >> "a.txt"
        done
    elif [[ "$piece" =~ [bg]r ]]; then
        row+=7
    elif [[ "$piece" =~ [bg]n ]]; then
        row+=5
    elif [[ "$piece" =~ [bg]b ]]; then
        row+=3
    else
        col=-1
    fi

    # printing twin pieces
    if [[ "$piece" =~ yr ]]; then
        echo "yr found"
        echo "$col"
    fi
    if [[ $col -gt 0 && $row -gt 0 && !($piece =~ [rbyg]p) ]]; then
        echo "test ${piece}"
        echo "<div class=\"piece-$piece\"></div>" >> "a.txt"
    fi
    return 0
}

g() {
    echo "running g"
    echo ".piece-${piece} {
  background-image: url("../assets/${piece}.png");
  position: absolute;
  z-index: 10;
  text-align: center;
  cursor: grab;
}" >> "b.txt"
}

names=$(echo| grep '[bgry][qknrpb]\.png' "afiles.txt" | sed 's/\.[^.]*$//')

echo "$names" > "names.txt" # text file now has front of file names
while read -r piece; do  
    echo "$piece"
    f
    g
done <names.txt

