#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <ctime>
#include <vector>

/* This class is for storing the data that is pulled from the .xml file */
class SetPoint
{
public:
    std::string startDateTime; // String that holds the start of the period.
    std::string endDateTime; // String that holds the end of period.
    std::string value1;  //String that reads 0 to 100.
};

namespace pt = boost::property_tree;  //Sets a namespace for the boost property tree.

int main()
{

/* Declaring an Initial Property Tree from a CAISO XML file*/
    pt::ptree propTree;  //declare a property tree
    pt::read_xml("EIM.xml", propTree);  //reads an XML file into the property tree.


/* Information for calling a specific data set from. */
    //For some reason these three properties were needed to guarantee a distinct data set.
    //Some data sets can be identified with less information. All data sets given in the
    //test document required only two of these items.
    std::string mridInterTieGiven = "TIE";  // Specifies set 6 or 7.
    std::string mridRegisteredInterTieGiven = "REGISTERED_INTERTIE_ITIE_ETIE"; //Specifies set 6 or 7.
    std::string scheduleTypeGiven = "FIRM"; // Specifies set 1, 2, 4, or 6.
    //End result of these settings is that set 6 will be specified.


/* Declaring String Variables related to the XML data */
    /* For storing the XML values for the uniquely identifying variables*/
        std::string scheduleType;
        std::string mridInterTie;
        std::string mridRegisteredInterTie;

    /* Fore recording the schedule data in to one data structure. */
        std::vector<SetPoint> schedule;  // Sets up a vector of my SetPoint class.
        SetPoint interval;  // Instantiates a SetPoint class.
        // This creates a vector of SetPoint classes, that I can use to store the XML scheduling data.


/* For Pulling the Year, Month and Day from the XML data header. */
    /* Initializing C strings for storing the data. */
        char year[5] = {'\0','\0','\0','\0','\0'}; //For storing the year.
        char month[3] = {'\0','\0','\0'}; //For storing the month.
        char day[3] = {'\0','\0','\0'}; //For storing the day of the month.

    /* Declaring a C++ string to pull the TimeDate data from the message header. */
        std::string TimeDate;
        TimeDate = propTree.get<std::string>("InterchangeScheduleData.MessageHeader.TimeDate");
        //std::cout << TimeDate << std::endl;

    /* Organizing the message header data into the Year, Month, and Date string */
        year[0] = TimeDate[0];
        year[1] = TimeDate[1];
        year[2] = TimeDate[2];
        year[3] = TimeDate[3];
        //printf("\n%s\n", year);
        month[0] = TimeDate[5];
        month[1] = TimeDate[6];
        //printf("\n%s\n", month);
        day[0] = TimeDate[8];
        day[1] = TimeDate[9];
        //printf("\n%s\n", day);
        time_t now = time(0);
        std::cout << now << std::endl;



/* This first loop counts through the data until I reach the data set I want. */
    int a = 0;  // Iteration counter.
    int b = 0;  // Iteration of data that I want to store (based on InterTie, RegisteredInterTie, and scheduleType).

    BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, propTree.get_child( "InterchangeScheduleData.MessagePayload") )
    // This construction is similar to a loop that goes through each set of nodes of data.
    {

        boost::property_tree::ptree subtree = node.second; // Creates a subtree, for accesssing lower ranking data.
        a++; // Counts my iteration of InterchangeSchedules. The test file has 7 of these InterchangeSchedules.

        if (node.first == "InterchangeSchedule"){
            mridInterTie = subtree.get<std::string>("InterTie.mrid"); // Saves the uniquely identifying data for the Interchange Schedules.
            scheduleType = subtree.get<std::string>("scheduleType");
            mridRegisteredInterTie = subtree.get<std::string>("RegisteredInterTie.mrid");

            // Now I check for a match.
            if ( (mridInterTie == mridInterTieGiven) && (mridRegisteredInterTie == mridRegisteredInterTieGiven) && (scheduleType == scheduleTypeGiven)){
                b = a; // If they match, set b = a.
            } // Closes the branch statement for matching the schedules.
        } // Closes the branch statement for indentifying the schedules.
    } // Closes the FOREACH.


/* Now the second loop counts through the right data set and records the data set into my schedule vector*/
    a = 0;  // Resetting iteration variable. Having set b==a at the right point, I now use a to count up to b.
    BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, propTree.get_child( "InterchangeScheduleData.MessagePayload") )
    {
    // Repeat of the first FOREACH, except now I know the right data set.

        boost::property_tree::ptree subtree = node.second;
        a++; // I count through again, looking for the place where a == b.

        if (b == a){ //I start saving the data into my schedule vector at this point, whe a == b.
            BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, propTree.get_child( "InterchangeScheduleData.MessagePayload.InterchangeSchedule") )
                // I am now sorting through a deeper section of the property tree.
                {
                    boost::property_tree::ptree subsubtree = node.second;  // This is a deeper subtree.

                    if(node.first == "IrregularDateTimePoint"){ // My data is held under identical headings of this type.
                        interval.value1 = subsubtree.get<std::string>("value1"); // Saving the data to my SetPoint class, called interval.
                        interval.startDateTime = subsubtree.get<std::string>("startDateTime");
                        interval.endDateTime = subsubtree.get<std::string>("endDateTime");
                        schedule.push_back(interval); // Storing the data in my schedule vector.
                    } // Closes my branch statement for storing data points.
                } // Closes my FOREACH for the subsubtree where b==a.
        } // Closes my branch statement for the right data set (b==a).
    } // Closes my FOREACH for the subtree which is counting for when b==a.

/* Prints my shedule data to the console */
//    for (int i = 0; i < schedule.size(); i++){
//        std::cout << schedule[i].value1 << std::endl;
//        std::cout << schedule[i].startDateTime << std::endl;
//        std::cout << schedule[i].endDateTime << std::endl;
//    }

    return 0;
}
