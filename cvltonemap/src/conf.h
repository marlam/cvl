/*
 * conf.h
 *
 * This file is part of cvlview, an image viewer using the CVL library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file conf.h
 * \brief Configuration file handling.
 *
 * Configuration file handling.
 */


#include "err.h"


#ifndef CONF_H
#define CONF_H

/**
 * Simple configuration file handling. 
 *
 * A configuration file stores a bunch of name/value pairs (both are strings
 * without control characters, names must not contain spaces).
 *
 * Get the values you are interested in via their name, or put new name/value
 * pairs into the configuration. If a pair with the same name existed before, it
 * will be overwritten. If you cannot parse the value string or if it contains
 * unexpected values, you should just treat it as if it did not exist.
 *
 * When you are sure that you have accessed all values that you are interested
 * in with get or put, you might want to call remove_cruft() to remove all pairs
 * that you did not access. This is to prevent configuration files growing too 
 * large because of pairs that were only supported in obsolete versions of your 
 * program.
 */
class Conf
{
    private:
	char **_name;
	char **_value;
	bool *_cruft;
	int _length;
	int _size;

	void dump() const;	// only for debugging
	void erase();

    public:
	Conf();
	~Conf();

	/**
	 * \param filename	Name of the file to read the configuration from.
	 */
	void load(const char *filename) throw (err);

	/**
	 * \param filename	Name of the file to save the configuration to.
	 */
	void save(const char *filename) const throw (err);

	/**
	 * \param name		Name of the name/value pair.
	 * \returns 		The value, of NULL if not found
	 */
	const char *get(const char *name);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param value		Value of the name/value pair.
	 */
	void put(const char *name, const char *value);

	/**
	 * This function removes all name/value pairs that were not yet found
	 * via a call to \a get().
	 */
	void remove_cruft();


	/* Convenience functions. */

	/**
	 * \param name		Name of the name/value pair.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to an integer, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to an
	 * integer.
	 */
	const char *get(const char *name, const char *safe_default);

	/**
	 * \param name		Name of the name/value pair.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to an integer, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to an
	 * integer.
	 */
	int get(const char *name, int safe_default);

	/**
	 * \param name		Name of the name/value pair.
	 * \param min		A lower bound.
	 * \param max		An upper bound.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to an integer, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to an
	 * integer or the integer is smaller than \a min or grater than \a max.
	 */
	int get(const char *name, int min, int max, int safe_default);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to a double, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to a
	 * double.
	 */
	double get(const char *name, double safe_default);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param min		A lower bound.
	 * \param max		An upper bound.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to a double, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to a
	 * double or the double is smaller than \a min or grater than \a max.
	 */
	double get(const char *name, double min, double max, double safe_default);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param safe_default	A default value.
	 * \returns 		The value.
	 *
	 * Returns the value transformed to a bool, or \a safe_default if 
	 * \a name was not found or the value could not be transformed to a
	 * bool. Valid strings are "on"/"off", "1"/"0", "true"/false".
	 */
	bool get(const char *name, bool safe_default);	
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param value		Value of the name/value pair.
	 */
	void put(const char *name, int value);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param value		Value of the name/value pair.
	 */
	void put(const char *name, double value);
	
	/**
	 * \param name		Name of the name/value pair.
	 * \param value		Value of the name/value pair.
	 */
	void put(const char *name, bool value);
};

#endif
