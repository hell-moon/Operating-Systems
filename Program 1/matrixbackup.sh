#!/bin/bash
# For this script,
# $1 is the matrix operation
# $2, $3 is each input passed to the script (matrices)
# example: 	./matrix [$1: OPERATION] [$2: MATRIX] [$3: MATRIX]

tempCol="tempcolfile"
tempRow="temprowfile"
tempMean="tempmeanfile"
tempSum="tempsumfile"

function perror(){
    # NAME
    #   perror - print a stack trace and error message
    # SYNOPSIS
    #   perror [STRING]
    # DESCRIPTION
    #   Echoes the STRING(s) to standard error with a stack trace for debugging.
    #   The value of the status flag $? is not changed by perror.
    # AUTHOR
    #   Written by Ryan Gambord (gambordr@oregonstate.edu)
    # source: https://oregonstate.instructure.com/courses/1719569/pages/block-1
    
    status=$? # Capture status of previous command
    # echo -e "\e[36mTraceback (most recent call last):\e[0m" >&2
    i=${#BASH_LINENO[@]} # Get number of items in stack trace
    
    # This section prints a stack trace of the current execution stack
    while
    [ $i -gt 0 ] # Iterate to the top of the stack
    do
        until # Loop until we get to the bottom of caller stack (this is safer than offsetting $i)
        ((i--))
        info=($(caller $i)) # Get caller info
        do :; done # Do nothing
        
        # echo "  File \"${info[2]}\", line ${info[0]}, in ${info[1]}()" >&2 # Print caller info
        if [ $i -ne 0 ] # Check if we're at the top of the stack (perror call is at top)
        then
            echo "    "$(head "${info[2]}" -n ${info[0]} | tail -n 1) >&2 # Print trace if not at top
        else
            echo -en "\e[31mERROR\e[0m: " >&2 # Print error message if at top
            [ $# -gt 0 ] && echo "$*" >&2 || echo "(no error message specified)" >&2
        fi
    done
    return $status
}

dims(){
    # NAME
    # dims - print the dimensions of a passed matrix
    # SYNOPSIS
    # dims [MATRIX]
    # DESCRIPTION
    # prints "ROWS COLS" (Space separated!) to stdout, nothing to stderr, and returns 0.
    # Assume MATRIX is valid:
    # * Valid matrix has columns separated by tab (^I) and each row ends with a newline character ($)
    # AUTHOR
    # Written by Xiao Kuang (kuangx@oregonstate.edu)
    # -----------------------------------------------------------------------------

    rowNumber=0
    colNumber=0
    # count number of rows
    while read 
    do
        rowNumber=$(($rowNumber + 1))
    done < $1
    
    colNumber=$(head -n 1 $1 | wc -w)
    echo $rowNumber $colNumber

    return 0
}

transpose(){
	# NAME
    # transpose - reflect the elements of the matrix along the main diagonal
    # SYNOPSIS
    # transpose [MATRIX]
    # DESCRIPTION
    # prints reflected matrix to stdout, nothing to stderr, and returns 0.
	# 		* An MxN matrix will become an NxM matrix and the values along the main diagonal will remain unchanged.
    # Assume MATRIX is valid:
    # 		* Valid matrix has columns separated by tab (^I) and each row ends with a newline character ($)
    # AUTHOR
    # Written by Xiao Kuang (kuangx@oregonstate.edu)
    # -----------------------------------------------------------------------------

	# starting from left, get a column, and paste as a row into temp file
	# get number of columns (count tabs in each column + 1)
	colcount=$(($( head -n 1 $1 | grep -o $'\t' | wc -l )+1))
	#         # column 1, 2, 3, ... n even columns are tab characters, so skip those
	# coltracker=1
	# while [read]
	# do
	# 	cut -c $coltracker $1 | 

	# done < $1
    # get number of \t characters in one row and add 1 to calculate the number of columns
  
    # cut each column one at a time and paste as a single line 
    for (( i=1; i<="$colcount"; i++ ))
    do
        cut -f "$i" "$1" | paste -s
    done

    return 0
}

# -----------------------------------------------------------------------------
# operator passed as first argument (dims, transpose, mean, add, multiply)
# matrices passed as 2nd (required) and 3rd (optional) arguments

##################
# dims execution #
##################
# Prints error message to stderr, nothing to stdout and return value != 0 if:
# 		* Argument count is greater than 1 (e.g. `matrix dims m1 m2`).
# 		* Argument count is 1 but the file named by argument 1 is not readable (e.g. `matrix dims no_such_file`).
# Otherwise call dims and pass matrix ->
# 		prints "ROWS COLS" (Space separated!) to stdout, nothing to stderr, and returns 0.
if [ $1 = 'dims' ]
then
    # check and make sure not more than 2 arguments passed to script
    if [ $# -gt 2 ]
    then
        perror "Too many arguments.  Please only pass one matrix.  "
        exit 5
    elif [ $# -eq 1 ]
    then
		tmpinput=tmp
		cat > $tmpinput
		dims $tmpinput
        # perror "Not enough arguments.  Please pass a matrix. "
        # exit 5
    elif [ $# -eq 2 ]
    then
        # check to see if possible to read from $2
        if [ read < $2 ]
        then
            dims $2
            exit 0
        else
            perror "Cannot read file"
            exit 5
        fi
    fi
fi


#######################
# transpose execution #
#######################
# Prints error message to stderr, nothing to stdout and return value != 0 if:
#     	* Argument count is greater than 1 (e.g. `matrix transpose m1 m2`).
#     	* Argument count is 1 but the file named by argument 1 is not readable (e.g. `matrix transpose no_such_file`).
# Otherwise, call transpose and pass matrix ->
# 		prints the transpose of the input, in a valid matrix format to stdout, nothing to stderr, and returns 0.
if [ $1 = 'transpose' ]
then
    # check and make sure not more than 2 arguments passed to script
    if [ $# -gt 2 ]
    then
        perror "Too many arguments"
        exit 5
    elif [ $# -eq 1 ]
    then
		tmpinput=tmp
		cat > $tmpinput
		transpose $tmpinput
        # perror "Not enough arguments.  Please pass a matrix. "
        # exit 5
    elif [ $# -eq 2 ]
    then
        # check to see if possible to read from $2
        if [ read < $2 ]
        then
            transpose $2
            exit 0
        else
            perror "Cannot read file"
            exit 5
        fi
    fi
fi


##################
# mean execution #
##################
# Prints error message to stderr, nothing to stdout and return value != 0 if:
#     	* Argument count is greater than 1 (e.g. `matrix mean m1 m2`).
#     	* Argument count is 1 but the file named by argument 1 is not readable (e.g. `matrix mean no_such_file`).
# Otherwise, call mean and pass matrix ->
# 		prints a row vector mean of the input matrix, in a valid matrix format to stdout, nothing to stderr, and returns 0.
# All values must round to the nearest integer, with ***.5 values rounded away from zero.
if [ $1 = 'mean' ]
then
    # check and make sure not more than 2 arguments passed to script
    if [ $# -gt 2 ]
    then
        perror "Too many arguments"
        exit 5
    elif [ $# -eq 1 ]
    then
        perror "Not enough arguments.  Please pass a matrix. "
        exit 5
    elif [ $# -eq 2 ]
    then
        # check to see if possible to read from $2
        if [ read < $2 ]
        then
            mean $2
            exit 0
        else
            perror "Cannot read file"
            exit 5
        fi
    fi
fi

##################
# add execution #
##################
# Prints error message to stderr, nothing to stdout and return value != 0 if:
#     	* Argument count does not equal 2 (e.g. `matrix add m1 m2 m3` or `matrix add m1`).
#     	* Argument count is 2 but the file named by either argument is not readable (e.g. `matrix add m1 no_such_file`).
#     	* The dimensions of the input matrices do not allow them to be added together following the rules of matrix addition.
# Otherwise, call add and pass two matrices ->
# 		prints the sum of both matricies, in a valid matrix format to stdout, nothing to stderr, and returns 0.
if [ $1 = 'add' ]
then
    # check and make sure not more than 2 arguments passed to script
    if [ $# -gt 3 ]
    then
        perror "Too many arguments. Please pass two matrices."
        exit 5
    elif [ $# -lt 3 ]
    then
        perror "Not enough arguments.  Please pass two matrices. "
        exit 5
    elif [ $# -eq 3 ]
    then
        # check to see if possible to read from $2
        if [ read < $2 ]
        then
			# check to see if possible to read from $3
            if [ read < $3 ]
            then
				# check to see if sizes are compatible for addition of matrices
				# TODO DO THIS SHIT RIGHT HERE, CHECK IF $2 AND $3 ARE ABLE TO DO MATRIX ADDITION
				if [  ] 
				then
					add $2 $3
					exit 0
				else
					perror "Matrices sizes cannot be added.  Please verify arguments passed."
					exit 5
				fi
                
			else
				perror "Cannot read file 2"
				exit 5
            fi
        else
            perror "Cannot read file 1"
            exit 5
        fi
    fi
fi

######################
# multiply execution #
######################
if [ $1 = 'multiply' ]
then
    # check and make sure not more than 2 arguments passed to script
    if [ $# -gt 3 ]
    then
        perror "Too many arguments. Please pass two matrices."
        exit 5
    elif [ $# -lt 3 ]
    then
        perror "Not enough arguments.  Please pass two matrices. "
        exit 5
    elif [ $# -eq 3 ]
    then
        # check to see if possible to read from $2
        if [ read < $2 ]
        then
			# check to see if possible to read from $3
            if [ read < $3 ]
            then
				# check to see if sizes are compatible for multiplication of matrices
				# TODO DO THIS SHIT RIGHT HERE, CHECK IF $2 AND $3 ARE ABLE TO DO MATRIX MULTIPLICATION
				if [  ] 
				then
					add $2 $3
					exit 0
				else
					perror "Matrices sizes cannot be multiplied.  Please verify arguments passed."
					exit 5
				fi
                
			else
				perror "Cannot read file 2"
				exit 5
            fi
        else
            perror "Cannot read file 1"
            exit 5
        fi
    fi
fi

