# By Laurentiu Stefan 343C3

function compare_files {
    # Ignore trailing whitespaces while comparing
    diff -Z $1 $2 &> /dev/null
    return $?
}

pad=$(printf '%.1s' "."{1..70})
padlength=71

function print_result {
    printf "%s" "$1"
    printf "%*.*s" 0 $((padlength - ${#1} - ${#2} )) "$pad"
    printf "%s\n" "$2"
}

function check_prerequisites {
    # Check if makefile exists
    ls . | grep -i "makefile" &> /dev/null

    if [ $? -ne 0 ]; then
        echo -e "Makefile file not found! Stopping testing!"
        exit 1
    fi

    makefile_name=$(ls . | grep -i makefile | cut -f 1 -d " ")

    # Check for makefile build, run, clean rules
    cat $makefile_name | grep -i "build:" &> /dev/null
    if [ $? -ne 0 ]; then
        echo -e "[MAKEFILE]: BUILD rule not found! Stopping testing!"
        exit 1
    fi

    cat $makefile_name | grep -i "run:" &> /dev/null
    if [ $? -ne 0 ]; then
        echo -e "[MAKEFILE]: RUN rule not found! Stopping testing!"
        exit 1
    fi

    cat $makefile_name | grep -i "clean:" &> /dev/null
    if [ $? -ne 0 ]; then
        echo -e "[MAKEFILE]: CLEAN rule not found! Stopping testing!" exit 1
    fi
}

function check_tasks {
    total_points=0
    tests_weight=(5 5 5 5 5 5 5 5 5 5 10 10 5 5)

    rm output/* &> /dev/null
    make clean &> /dev/null
    make build &> /dev/null

    if [ $? -ne 0 ]; then
        echo "[MAKEFILE] Makefile build rule failed! Stopping testing!"
        exit 1
    fi

    # Check if the executable exists
    ls . | grep "fsminishell" &> /dev/null

    if [ $? -ne 0 ]; then
        echo -e "fsminishell executable not found! Stopping testing!"
        exit 1
    fi

    echo "===========================NORMAL TESTS================================"

    for i in $(seq 1 14)
    do
        current_description=""

        if [ $i -lt 7 ]; then
            current_description="(touch mkdir ls)"
        elif [ $i -lt 11 ]; then
            current_description="(pwd cd)"
        elif [ $i -lt 13 ]; then
            current_description="(tree)"
        else
            current_description="(rm rmdir)"
        fi

        current_input="./input/test${i}.in"
        current_output="./output/test${i}.out"
        current_ref="./ref/test${i}.ref"

        # Running the executable
        ./fsminishell < $current_input > $current_output

        # Compare the output with the reference
        compare_files $current_output $current_ref

        if [ $? -eq 0 ]; then
            print_result "Test $i $current_description" "OK (${tests_weight[i - 1]}p)"
            ((total_points=total_points+${tests_weight[i - 1]}))
        else
            print_result "Test $i $current_description" "FAILED"
        fi
    done

    # Running the valgrind test
    valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./fsminishell < ./input/testValgrind.in &> /dev/null

    if [ $? -eq 0 ]; then
        print_result "Test memcheck (rm rmdir)" "OK (10p)"
        ((total_points=total_points+10))
    else
        print_result "Test memcheck (rm rmdir)" "FAILED"
    fi

    echo ""
    print_result "Total TESTS" "[${total_points}p / 90p]"
    ((total_points=total_points+current_test_points))

    echo "======================================================================="
    echo ""

    make clean &> /dev/null
}

function check_readme {
    echo "=============================README===================================="

    ls . | grep -i "readme" &> /dev/null

    if [ $? -eq 0 ]; then
        print_result "Checking for README file" "OK (10p for now)"
        readme_points=10
        ((total_points=total_points+10))
    else
        readme_points=0
        print_result "Checking for README file" "FAILED"
    fi

    echo ""
    print_result "Total README" "[${readme_points}p / 10p]"
    echo "======================================================================="
    echo ""
}

function check_bonus {
    echo "===========================BONUS TESTS================================="

    make build &> /dev/null

    tests_weight=(10 10)

    for i in $(seq 15 16)
    do
        current_input="./input/test${i}.in"
        current_output="./output/test${i}.out"
        current_ref="./ref/test${i}.ref"

        # Running the executable
        ./fsminishell < $current_input > $current_output

        # Compare the output with the reference
        compare_files $current_output $current_ref

        if [ $? -eq 0 ]; then
            print_result "Test $i (find)" "OK (${tests_weight[i - 15]}p)"
            ((total_points=total_points+${tests_weight[i - 15]}))
        else
            print_result "Test $i (find)" "FAILED"
        fi
    done

    echo ""
    print_result "Total BONUS TESTS" "[$(expr $total_points - 100)p / 20p]"
    ((total_points=total_points+current_test_points))

    echo "======================================================================="
    echo ""

    make clean &> /dev/null
}

function check_style {
    echo "==============================STYLE===================================="

    ./style.pl --ignore SPDX_LICENSE_TAG,NEW_TYPEDEFS,CONST_STRUCT --no-tree --terse -f *.c *.h > output/styleErrors.txt # changed name from checkpatch.pl to avoid confusion with check.sh

    if [ $? -eq 0 ]; then
        print_result "Checking for coding style" "OK"
        style_points=0
        ((total_points=total_points+0))
    else
        style_points=-10
        ((total_points=total_points-10))
        print_result "Checking for coding style" "FAILED (-10p for now)"
    fi

    echo ""
    print_result "Total STYLE" "[${style_points}p / 0p]"
    echo "======================================================================="
    echo ""
}

# "Main" code starts here
check_prerequisites

check_tasks

check_readme

if [ $total_points -eq 100 ]; then
    check_bonus
else
    echo "You don't have maximum points! Not checking the bonus!"
    echo ""
fi

check_style

if [ $total_points -eq 120 ]; then
    echo 'Congratulations! You have maximum points!'
    echo ""
fi

echo "TOTAL: ${total_points}/120"
