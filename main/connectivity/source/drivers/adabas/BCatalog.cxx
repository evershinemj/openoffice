/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_connectivity.hxx"
#include "adabas/BCatalog.hxx"
#include "adabas/BConnection.hxx"
#include "adabas/BGroups.hxx"
#include "adabas/BUsers.hxx"
#include "adabas/BTables.hxx"
#include "adabas/BViews.hxx"
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <comphelper/types.hxx>


// -------------------------------------------------------------------------
using namespace connectivity;
using namespace connectivity::adabas;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
// -----------------------------------------------------------------------------
OAdabasCatalog::OAdabasCatalog(SQLHANDLE _aConnectionHdl, OAdabasConnection* _pCon) : connectivity::sdbcx::OCatalog(_pCon)
				,m_pConnection(_pCon)
				,m_aConnectionHdl(_aConnectionHdl)
{
}
// -----------------------------------------------------------------------------
::rtl::OUString OAdabasCatalog::buildName(const Reference< XRow >& _xRow)
{
	::rtl::OUString sName;
	sName = _xRow->getString(2);
    if ( sName.getLength() )
        sName += OAdabasCatalog::getDot();
	sName += _xRow->getString(3);
    

	return sName;
}
// -----------------------------------------------------------------------------
void OAdabasCatalog::fillVector(const ::rtl::OUString& _sQuery,TStringVector& _rVector)
{
    Reference< XStatement > xStmt = m_pConnection->createStatement(  );
    OSL_ENSURE(xStmt.is(),"OAdabasCatalog::fillVector: Could not create a statement!");
	Reference< XResultSet > xResult = xStmt->executeQuery(_sQuery);

	fillNames(xResult,_rVector);
	::comphelper::disposeComponent(xStmt);

}
// -------------------------------------------------------------------------
void OAdabasCatalog::refreshTables()
{
	TStringVector aVector;
	{
		Sequence< ::rtl::OUString > aTypes(1);
		aTypes[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%"));
		Reference< XResultSet > xResult = m_xMetaData->getTables(Any(),
																::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")),
																::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")),
																aTypes);
		fillNames(xResult,aVector);
	}

	if(m_pTables)
		m_pTables->reFill(aVector);
	else
		m_pTables = new OTables(m_xMetaData,*this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OAdabasCatalog::refreshViews()
{
	TStringVector aVector;
    static const ::rtl::OUString s_sView(RTL_CONSTASCII_USTRINGPARAM("SELECT DISTINCT NULL,DOMAIN.VIEWDEFS.OWNER, DOMAIN.VIEWDEFS.VIEWNAME FROM DOMAIN.VIEWDEFS"));
	fillVector(s_sView,aVector);

	if(m_pViews)
		m_pViews->reFill(aVector);
	else
		m_pViews = new OViews(m_xMetaData,*this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OAdabasCatalog::refreshGroups()
{
	TStringVector aVector;
    static const ::rtl::OUString s_sGroup(RTL_CONSTASCII_USTRINGPARAM("SELECT DISTINCT NULL,NULL,GROUPNAME FROM DOMAIN.USERS WHERE GROUPNAME IS NOT NULL AND GROUPNAME <> ' '"));
    fillVector(s_sGroup,aVector);
	if(m_pGroups)
		m_pGroups->reFill(aVector);
	else
		m_pGroups = new OGroups(*this,m_aMutex,aVector,m_pConnection,this);
}
// -------------------------------------------------------------------------
void OAdabasCatalog::refreshUsers()
{
	TStringVector aVector;
    static const ::rtl::OUString s_sUsers(RTL_CONSTASCII_USTRINGPARAM("SELECT DISTINCT NULL,NULL,USERNAME FROM DOMAIN.USERS WHERE USERNAME IS NOT NULL AND USERNAME <> ' ' AND USERNAME <> 'CONTROL'"));
    fillVector(s_sUsers,aVector);

	if(m_pUsers)
		m_pUsers->reFill(aVector);
	else
		m_pUsers = new OUsers(*this,m_aMutex,aVector,m_pConnection,this);
}
// -------------------------------------------------------------------------
const ::rtl::OUString& OAdabasCatalog::getDot()
{
	static const ::rtl::OUString sDot(RTL_CONSTASCII_USTRINGPARAM("."));
	return sDot;
}
// -----------------------------------------------------------------------------
void OAdabasCatalog::correctColumnProperties(sal_Int32 /*_nPrec*/, sal_Int32& _rnType,::rtl::OUString& _rsTypeName)
{
	switch(_rnType)
	{
	case DataType::DECIMAL:
		{
            static const ::rtl::OUString sDecimal(RTL_CONSTASCII_USTRINGPARAM("DECIMAL"));
			if(_rnType == DataType::DECIMAL && _rsTypeName == sDecimal)
				_rnType = DataType::NUMERIC;
		}
		break;
	case DataType::FLOAT:
		//	if(_nPrec >= 16)
		{
			static const ::rtl::OUString sDouble(RTL_CONSTASCII_USTRINGPARAM("DOUBLE PRECISION"));
			_rsTypeName = sDouble;
			_rnType = DataType::DOUBLE;
		}
//		else if(_nPrec > 15)
//		{
//			static const ::rtl::OUString sReal = ::rtl::OUString::createFromAscii("REAL"); 
//			_rsTypeName = sReal;
//			_rnType = DataType::REAL;
//		}
		break;
	}	
}
// -----------------------------------------------------------------------------

