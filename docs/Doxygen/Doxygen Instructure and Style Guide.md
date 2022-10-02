# Doxygen

To generate Doxygen documentation in PDF form you need to install the following
packages:

-doxygen
-texlive-latex-base
-texlive-latex-recommended 
-texlive-fonts-recommended
-texlive-latex-extra

Style Guide:
    All Doxygen documentation will be done with the Javadoc style.
    Add Classes to appropriate subgroup.
    Each file will have the same format at the top
    Documentation of member functions will go in header files.
    All public and private methods, along with data members need to have
    Doxygen style documentation.

Run from repo root:
doxygen ./docs/Doxygen/Doxyfile