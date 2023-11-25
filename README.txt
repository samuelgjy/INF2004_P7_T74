
README
SETUP
Create a New Folder:

Navigate to C:.../pico-v1.5.1/pico-examples.
Create a new folder, for example, "assignment."
Pull the Repository:

Clone the repository into C:.../pico-v1.5.1/pico-examples/(your folder name here).
Modify CMakeLists.txt:

Open the file C:.../pico-v1.5.1/pico-examples/CMakeLists.txt.
Add the following line at the bottom:
cmake

add_subdirectory(your folder name here)
Create New CMakeLists.txt:

In the newly created folder (C:.../pico-v1.5.1/pico-examples/(your folder name here)), create a new file named CMakeLists.txt.
Add the following content:
cmake

if (NOT PICO_NO_HARDWARE)
    add_subdirectory(INF2004-T74-P7)
endif ()
Verify:

Ensure that you can see the "INF2004-T74-P7" folder from the GitHub repository in your newly created folder.