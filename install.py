from subprocess import STDOUT, check_call
import os
import sys

print("Giraffe - Installer")

REQUIRE_VALIDATION = True

def GetInputOption(options, prompt="$: "):
    options = options.copy()
    options.append("Exit")
    print();
    while True:
        for index, option in enumerate(options):
            print(f"{index + 1}. {option}")
        try:
            choice = int(input(prompt))
            if choice > 0 and choice <= len(options):
                if choice == len(options):
                    print("Exiting")
                    sys.exit(0)
                return choice
        except Exception as e:
            pass
        print("Invalid choice")
        
def Verify(promt):
    if not REQUIRE_VALIDATION:
        return True
    print(promt)
    while True:
        choice = input("y/n: ")
        if choice.lower() == "y":
            return True
        elif choice.lower() == "n":
            return False
        else:
            print("Invalid input")

def InstallFlightSoftwarePrereqs():
    packages = ["build-essential", "libi2c-dev"]
    
    string_packages = "".join([f"{package} " for package in packages])
    if not Verify(f"Do you want to install the following packages: {string_packages}?"):
        return
    # update apt
    check_call(["sudo", "apt", "update"])
    # install packages
    for package in packages:
        check_call(["sudo", "apt", "install", "-y", package])
    print("Done")

def FlightSoftwareMenu():
    options = ["Install Flight Software Pre-requisites", "Go Back"]
    while True:
        choice = GetInputOption(options)
        if choice == 1:
            InstallFlightSoftwarePrereqs()
        elif choice == len(options):
            return

def MainMenu():
    options = ["Install Flight Software"]
    while True:
        choice = GetInputOption(options)
        if choice == 1:
            FlightSoftwareMenu()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        MainMenu()
    
    REQUIRE_VALIDATION = False
    
    if (sys.argv[1] == "gfs_pr"):
        print("Installing Flight Software Pre-requisites")
        InstallFlightSoftwarePrereqs()
    else:
        sys.exit(1);