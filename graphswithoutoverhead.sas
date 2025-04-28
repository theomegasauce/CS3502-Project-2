/* Step 1: Import the CSV file */
proc import datafile="/home/u63980093/Project 2 CS3502/scheduling_results.csv"
    out=scheduling_results
    dbms=csv
    replace;
    guessingrows=MAX;
run;

/* Step 2: View structure */
proc contents data=scheduling_results;
run;

/* Step 2b: Print a sample */
proc print data=scheduling_results;
run;

proc sgplot data=sorted_awt;
    styleattrs datacolors=(CXA6CEE3 CX1F78B4 CXB2DF8A CX33A02C CXFDBF6F CXFF7F00 CXCAB2D6);
    vbar 'Test Category'n / response=AWT group=Algorithm groupdisplay=cluster datalabel;
    title "Average Waiting Time (AWT) by Test Category and Algorithm";
    xaxis label="Test Category";
    yaxis label="Average Waiting Time";
run;

/* Step 4b: Bar Graph - Average Turnaround Time (ATT) by Test Category and Algorithm */
proc sgplot data=sorted_att;
    styleattrs datacolors=(CXA6CEE3 CX1F78B4 CXB2DF8A CX33A02C CXFDBF6F CXFF7F00 CXCAB2D6);
    vbar 'Test Category'n / response=ATT group=Algorithm groupdisplay=cluster datalabel;
    title "Average Turnaround Time (ATT) by Test Category and Algorithm";
    xaxis label="Test Category";
    yaxis label="Average Turnaround Time";
run;
