# Typhoon framework

A simple structure to easily TDD in C with [Criterion](https://github.com/Snaipe/Criterion) that uses [Docker](https://www.docker.com/) to automates the building phase.

## Ideology

Typhoon enable the creation of multiple subprograms and ensures their testability with Criterion with a single command.  
This enables CI tools like Jenkins and several metrics to be run once on the whole code. 

## Workflow

The general localisation of the code is very important in order for Typhoon to work.  
Place your tests and production code in the `tests` and `src` folders, respectively.  
Place your main in the `main` folder, naming it *subprogram*-main.c *(Note that the actual name of the file is totally irrelevant here. The suggested notation helps for clarity)*  
As long as you respect this, the building process will work perfectly.  
Although I would recommand to create identical subdirectories under `tests` and `src` for each module in order to keep your program human-readable and friendly to the eyes, the internal structure of these folders is irrelevant compilation-wise. Thus, as long as your code is under `src` and `tests`, you can do whatever you want.

From there, the Criterion tests will automatically run on the enterety of the code on the invocation of `runtest.sh`.  
  
To run a subprogram individually, you need to create a `[*subprogram*]-launch.sh` script.  
This subprogram-launch script should describe which module will be used for its subprogram. It uses `template-launch.sh` to assemble a custom `Makefile.am` file with the specified modules and builds the subprogram.  
** !!! IMPORTANT NOTE !!!   
THE SUBPROGRAM-LAUNCH TEMPLATE AND AUTOMATIC BUILD FUNCTIONNALITIES AREN'T YET IMPLEMENTED.**


## The structure

- project/
    - autotools/
        - autogen.sh
        - configure.ac
        - Templatefile.am
    - build/
        - ...
    - main/
        - subprogramA-main.c
        - subprogramB-main.c
    - util/
    - src/
        - moduleA/
        - moduleB/
    - tests/
        - moduleA/
        - moduleB/
    - showlogs.sh
    - build-docker.sh
    - runtest.sh
    - subprogramA-launch.sh
    - subprogramB-launch.sh
    - template-launch.sh

