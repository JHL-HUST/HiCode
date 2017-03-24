/**********************************************************************************

 Infomap software package for multi-level network clustering

 Copyright (c) 2013 Daniel Edler, Martin Rosvall
 
 For more information, see <http://www.mapequation.org>
 

 This file is part of Infomap software package.

 Infomap software package is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Infomap software package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Infomap software package.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************/


#ifndef PROGRAMINTERFACE_H_
#define PROGRAMINTERFACE_H_

#include <getopt.h>
#include <vector>
#include <deque>
#include <iosfwd>
#include <string>
#include <sstream>
#include <stdexcept>

/**
 *
 */


struct OptionConflictError : std::logic_error
{
	OptionConflictError(std::string const& s) : std::logic_error(s) {}
};

template<typename T>
bool stringToValue(std::string const& str, T& value)
{
	std::istringstream istream(str);
	return istream >> value;
}

class Str {
public:
	Str() {}
	template<class T> Str& operator<<(const T& t) {
		m_oss << t;
		return *this;
	}
	Str& operator<<(std::ostream& (*f) (std::ostream&)) {
		m_oss << f;
		return *this;
	}
	operator std::string() const {
		return m_oss.str();
	}
private:
	std::ostringstream m_oss;
};

struct Option
{
	Option(char shortName, std::string longName, std::string desc, bool requireArgument = false,
			std::string argName = "", std::string defaultValue = "")
	: shortName(shortName),
	  longName(longName),
	  description(desc),
	  requireArgument(requireArgument),
	  incrementalArgument(false),
	  argumentName(argName),
	  defaultValue(defaultValue)
	{}
	virtual ~Option() {}
	virtual bool parse(std::string const&) { return true; }
	virtual void set(bool value) {};
	virtual std::ostream& printValue(std::ostream& out) const { return out; }
	friend std::ostream& operator<<(std::ostream& out, const Option& option)
	{
		return option.printValue(out);
	}
	char shortName;
	std::string longName;
	std::string description;
	bool requireArgument;
	bool incrementalArgument;
	std::string argumentName;
	std::string defaultValue;
};

struct IncrementalOption : Option
{
	IncrementalOption(unsigned int& target, char shortName, std::string longName, std::string desc)
	: Option(shortName, longName, desc, false), target(target)
	{ incrementalArgument = true; }
	virtual bool parse(std::string const&  value) {	return ++target; }
	virtual void set(bool value) { if (value) { ++target; } else if (target > 0) {--target;} }
	virtual std::ostream& printValue(std::ostream& out) const { return out << target; }

	unsigned int& target;
};

template<typename T>
struct ArgumentOption : Option
{
	ArgumentOption(T& target, char shortName, std::string longName, std::string desc, std::string argName)
	: Option(shortName, longName, desc, true, argName), target(target)
	{}
	virtual bool parse(std::string const&  value) {	return stringToValue(value, target); }
	virtual std::ostream& printValue(std::ostream& out) const { return out << target; }

	T& target;
};

template<>
struct ArgumentOption<bool> : Option
{
	ArgumentOption(bool& target, char shortName, std::string longName, std::string desc)
	: Option(shortName, longName, desc, false), target(target)
	{}
	virtual bool parse(std::string const&  value) {	return target = true; }
	virtual void set(bool value) { target = value; }
	virtual std::ostream& printValue(std::ostream& out) const { return out << target; }

	bool& target;
};

struct TargetBase
{
	TargetBase(std::string variableName, std::string desc)
	: variableName(variableName), description(desc)
	{}
	virtual ~TargetBase() {}
	virtual bool parse(std::string const& value) = 0;

	std::string variableName;
	std::string description;
};

template<typename T>
struct Target : TargetBase
{
	Target(T& target, std::string variableName, std::string desc)
	: TargetBase(variableName, desc), target(target)
	{}

	virtual bool parse(std::string const&  value)
	{
		return stringToValue(value, target);
	}

	T& target;
};

class ProgramInterface
{
public:
	ProgramInterface(std::string name, std::string shortDescription, std::string version);
	virtual ~ProgramInterface();

	void addProgramDescription(std::string desc)
	{
		m_programDescription = desc;
	}

	template<typename T>
	void addNonOptionArgument(T& target, std::string variableName, std::string desc)
	{
		TargetBase* t = new Target<T>(target, variableName, desc);
		m_nonOptionArguments.push_back(t);
	}

	void addOptionArgument(char shortName, std::string longName, std::string description)
	{
		m_optionArguments.push_back(new Option(shortName, longName, description));
	}

	void addIncrementalOptionArgument(unsigned int& target, char shortName, std::string longName, std::string description)
	{
		Option* o = new IncrementalOption(target, shortName, longName, description);
		m_optionArguments.push_back(o);
	}

	void addOptionArgument(bool& target, char shortName, std::string longName, std::string description)
	{
		Option* o = new ArgumentOption<bool>(target, shortName, longName, description);
		m_optionArguments.push_back(o);
	}

	// Without shortName
	void addOptionArgument(bool& target, std::string longName, std::string description)
	{
		Option* o = new ArgumentOption<bool>(target, '\0', longName, description);
		m_optionArguments.push_back(o);
	}

	template<typename T>
	void addOptionArgument(T& target, char shortName, std::string longName, std::string description, std::string argumentName)
	{
		Option* o = new ArgumentOption<T>(target, shortName, longName, description, argumentName);
		m_optionArguments.push_back(o);
	}

	// Without shortName
	template<typename T>
	void addOptionArgument(T& target, std::string longName, std::string description, std::string argumentName)
	{
		Option* o = new ArgumentOption<T>(target, '\0', longName, description, argumentName);
		m_optionArguments.push_back(o);
	}




	void parseArgs(int argc, char** argv);

private:
	void exitWithUsage();
	void exitWithVersionInformation();
	void exitWithError(std::string message);

	std::vector<option> m_longOptions; // struct option defined in <getopt.h>
	std::deque<Option*> m_optionArguments;
	std::deque<TargetBase*> m_nonOptionArguments;
	std::string m_programName;
	std::string m_shortProgramDescription;
	std::string m_programVersion;
	std::string m_programDescription;
	std::string m_executableName;
	bool m_displayHelp;
	bool m_displayVersion;
	bool m_negateNextOption;
};


#endif /* ARGPARSER_H_ */
