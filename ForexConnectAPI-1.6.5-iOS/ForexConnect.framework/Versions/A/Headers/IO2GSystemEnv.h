#pragma once

/** System Environment.*/
class IO2GSystemEnv
{
 public:
     /** Creates a string with information about current environment.
     *  @return a string in CSV format.
     */
    virtual std::string getEnvInfo() = 0;
};