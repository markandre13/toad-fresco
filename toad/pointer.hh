/*
 * TOAD -- A Simple and Powerful C++ GUI Toolkit for X-Windows
 * Copyright (C) 1996-98 by Mark-André Hopf
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GSmartPointer
#define GSmartPointer GSmartPointer

/*
	ATTENTION CORBA HACKERS:

	As far as i remember CORBA code like

			Object *p = new Object();
			Object_var v1 = p;
			Object_var v2 = p;

	will cause a segmentation fault since the reference counter isn't
	incremented by `ObjVar<T>& ObjVar::operator=( T* ptr )'.
	But TOADs `GSmartPointer<T>& GSmartPointer::operator =(T *p)' will
	increment it so code like

			TObject *p = new TObject();
			PObject v1 = p;
			PObject v2 = p;

  will destroy the object when BOTH PPointer objects get destroyed or
  loose their reference on the object. [MAH]
*/

#include <iostream>

#if 1
class TSmartReference
{
	public:
		TSmartReference() {
			_toad_ref_cntr = 0;
		}
		
		~TSmartReference() {
			if (_toad_ref_cntr!=0) {
				cerr << "warning: object with pending references destroyed" << endl;
				// Another idea would be to find all associated smart pointers
				// and set 'em to NULL.
			}
		}
//	private:
		unsigned _toad_ref_cntr;
};

template <class T>
class GSmartPointer
{
		T* _ptr;

	public:
		GSmartPointer() {
			_ptr = NULL;
		}

		GSmartPointer(T* p) {
			_ptr = NULL;
			_Set(p);
		}

		GSmartPointer(const GSmartPointer<T>& p) {
			_ptr = NULL;
			_Set(p._ptr);
		}

		~GSmartPointer() {
			_Set(NULL);
		}
		
		GSmartPointer<T>& operator =(T *p) {
			_Set(p);
			return *this;
		}
		
		GSmartPointer<T>& operator =(const GSmartPointer<T>& p) {
			_Set(p._ptr);
			return *this;
		}

		T* operator->() const {
			return _ptr;
		}
		
		operator T*() const {
			return _ptr;
		}
		
	private:
		void _Set(T *p) {
			if (_ptr) {
				if ((--_ptr->_toad_ref_cntr)==0) {
					delete _ptr;
				}
			}
			_ptr = p;
			if (_ptr) {
				_ptr->_toad_ref_cntr++;
			}
		}
};

#else

class TSmartReference
{
	public:
		TSmartReference() {
			cout << "TSmartReference" << endl;
			_toad_ref_cntr = 0;
		}
		
		~TSmartReference() {
			cout << "~TSmartReference" << endl;
			if (_toad_ref_cntr!=0) {
				cerr << "warning: object with references destroyed" << endl;
				// Another idea would be to find all associated smart pointers
				// and set 'em to NULL.
			}
		}
//	private:
		unsigned _toad_ref_cntr;
};

template <class T>
class GSmartPointer
{
		T* _ptr;

	public:
		GSmartPointer() {
			cout << "GSmartPointer()" << endl;
			_ptr = NULL;
		}

		GSmartPointer(T* p) {
			cout << "GSmartPointer(T*)" << endl;
			_ptr = NULL;
			_Set(p);
		}

		GSmartPointer(const GSmartPointer<T>& p) {
			cout << "GSmartPointer(const GSmartPointer<T>& p)" << endl;
			_ptr = NULL;
			_Set(p._ptr);
		}

		~GSmartPointer() {
			cout << "~GSmartPointer()" << endl;
			_Set(NULL);
		}
		
		GSmartPointer<T>& operator =(T *p) {
			cout << "T* operator =(T *p)" << endl;
			_Set(p);
			return *this;
		}
		
		GSmartPointer<T>& operator =(const GSmartPointer<T>& p) {
			cout << "GSmartPointer<T>& operator =(const GSmartPointer<T>& p)" << endl;
			_Set(p._ptr);
			return *this;
		}

		T* operator->() const {
			return _ptr;
		}
		
		operator T*() const {
			return _ptr;
		}
		
	private:
		void _Set(T *p) {
			if (_ptr) {
				if ((--_ptr->_toad_ref_cntr)==0) {
					cout << "reference == 0" << endl;
					delete _ptr;
				}
			}
			_ptr = p;
			if (_ptr) {
				_ptr->_toad_ref_cntr++;
			}
		}
};
#endif

#endif
