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
#include "precompiled_xmloff.hxx"
#include "MetaExportComponent.hxx"
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>

#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP
#include <com/sun/star/uno/Exception.hpp>
#endif
#include <com/sun/star/beans/PropertyAttribute.hpp>

// #110680#
//#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
//#include <comphelper/processfactory.hxx>
//#endif
#include <comphelper/genericpropertyset.hxx>
#include <rtl/ustrbuf.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlmetae.hxx>
#include "PropertySetMerger.hxx"
#include <tools/debug.hxx>

#include <unotools/docinfohelper.hxx>


using namespace ::com::sun::star;
using namespace ::xmloff::token;

// #110680#
XMLMetaExportComponent::XMLMetaExportComponent(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
		sal_uInt16 nFlags )
:	SvXMLExport( xServiceFactory, MAP_INCH, XML_TEXT, nFlags )
{
}

XMLMetaExportComponent::~XMLMetaExportComponent()
{
}

void SAL_CALL XMLMetaExportComponent::setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
	try
	{
		SvXMLExport::setSourceDocument( xDoc );
	}
	catch( lang::IllegalArgumentException& )
	{
		// allow to use document properties service without model access
		// this is required for document properties exporter
		mxDocProps =
            uno::Reference< document::XDocumentProperties >::query( xDoc );
		if( !mxDocProps.is() )
			throw lang::IllegalArgumentException();
	}
}

sal_uInt32 XMLMetaExportComponent::exportDoc( enum XMLTokenEnum )
{
	uno::Reference< xml::sax::XDocumentHandler > xDocHandler = GetDocHandler();

	if( (getExportFlags() & EXPORT_OASIS) == 0 )
	{
		uno::Reference< lang::XMultiServiceFactory > xFactory = getServiceFactory();
		if( xFactory.is() )
		{
			try
			{
				::comphelper::PropertyMapEntry aInfoMap[] =
				{
					{ "Class", sizeof("Class")-1, 0,
						&::getCppuType((::rtl::OUString*)0),
						beans::PropertyAttribute::MAYBEVOID, 0},
					{ NULL, 0, 0, NULL, 0, 0 }
				};
				uno::Reference< beans::XPropertySet > xConvPropSet(
					::comphelper::GenericPropertySet_CreateInstance(
							new ::comphelper::PropertySetInfo( aInfoMap ) ) );

				uno::Any aAny;
				aAny <<= GetXMLToken( XML_TEXT );
				xConvPropSet->setPropertyValue(
						::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Class")), aAny );

				uno::Reference< beans::XPropertySet > xPropSet =
					getExportInfo().is()
						?  PropertySetMerger_CreateInstance( getExportInfo(),
														  xConvPropSet )
						: getExportInfo();

				uno::Sequence< uno::Any > aArgs( 3 );
				aArgs[0] <<= xDocHandler;
				aArgs[1] <<= xPropSet;
				aArgs[2] <<= GetModel();

				// get filter component
				xDocHandler = uno::Reference< xml::sax::XDocumentHandler >(
					xFactory->createInstanceWithArguments(
						::rtl::OUString::createFromAscii("com.sun.star.comp.Oasis2OOoTransformer"),
						aArgs),
					uno::UNO_QUERY_THROW );

				SetDocHandler( xDocHandler );
			}
			catch( com::sun::star::uno::Exception& )
			{
				OSL_ENSURE( sal_False, "Cannot instantiate com.sun.star.comp.Oasis2OOoTransformer!\n");
			}
		}
	}


	xDocHandler->startDocument();
	{
#if 0
        GetAttrList().AddAttribute(
                GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_DC ),
                GetNamespaceMap().GetNameByKey( XML_NAMESPACE_DC ) );
        GetAttrList().AddAttribute(
                GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_META ),
                GetNamespaceMap().GetNameByKey( XML_NAMESPACE_META ) );
        GetAttrList().AddAttribute(
                GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_OFFICE ),
                GetNamespaceMap().GetNameByKey( XML_NAMESPACE_OFFICE ) );
#else
        const SvXMLNamespaceMap& rMap = GetNamespaceMap();
		sal_uInt16 nPos = rMap.GetFirstKey();
		while( USHRT_MAX != nPos )
		{
			GetAttrList().AddAttribute( rMap.GetAttrNameByKey( nPos ), rMap.GetNameByKey( nPos ) );
			nPos = GetNamespaceMap().GetNextKey( nPos );
		}
#endif

        const sal_Char* pVersion = 0;
        switch( getDefaultVersion() )
        {
        case SvtSaveOptions::ODFVER_LATEST: pVersion = "1.2"; break;
        case SvtSaveOptions::ODFVER_012: pVersion = "1.2"; break;
        case SvtSaveOptions::ODFVER_011: pVersion = "1.1"; break;
        case SvtSaveOptions::ODFVER_010: break;

        default:
            DBG_ERROR("xmloff::XMLMetaExportComponent::exportDoc(), unexpected odf default version!");
        }

        if( pVersion )
		    AddAttribute( XML_NAMESPACE_OFFICE, XML_VERSION,
                            ::rtl::OUString::createFromAscii(pVersion) );

			SvXMLElementExport aDocElem( *this, XML_NAMESPACE_OFFICE, XML_DOCUMENT_META,
						sal_True, sal_True );

        // NB: office:meta is now written by _ExportMeta
        _ExportMeta();
	}
	xDocHandler->endDocument();
	return 0;
}

void XMLMetaExportComponent::_ExportMeta()
{
    if (mxDocProps.is()) {
        ::rtl::OUString generator( ::utl::DocInfoHelper::GetGeneratorString() );
        // update generator here
        mxDocProps->setGenerator(generator);
        SvXMLMetaExport * pMeta = new SvXMLMetaExport(*this, mxDocProps);
        uno::Reference<xml::sax::XDocumentHandler> xMeta(pMeta);
        pMeta->Export();
    } else {
        SvXMLExport::_ExportMeta();
    }
}

// methods without content:
void XMLMetaExportComponent::_ExportAutoStyles() {}
void XMLMetaExportComponent::_ExportMasterStyles() {}
void XMLMetaExportComponent::_ExportContent() {}


uno::Sequence< rtl::OUString > SAL_CALL XMLMetaExportComponent_getSupportedServiceNames()
	throw()
{
	const rtl::OUString aServiceName(
		RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.XMLOasisMetaExporter" ) );
	const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
	return aSeq;
}

rtl::OUString SAL_CALL XMLMetaExportComponent_getImplementationName() throw()
{
	return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "XMLMetaExportComponent" ) );
}

uno::Reference< uno::XInterface > SAL_CALL XMLMetaExportComponent_createInstance(
		const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
	throw( uno::Exception )
{
	// #110680#
	// return (cppu::OWeakObject*)new XMLMetaExportComponent;
	return (cppu::OWeakObject*)new XMLMetaExportComponent(rSMgr, EXPORT_META|EXPORT_OASIS);
}

uno::Sequence< rtl::OUString > SAL_CALL XMLMetaExportOOO_getSupportedServiceNames()
	throw()
{
	const rtl::OUString aServiceName(
		RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.XMLMetaExporter" ) );
	const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
	return aSeq;
}

rtl::OUString SAL_CALL XMLMetaExportOOO_getImplementationName() throw()
{
	return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "XMLMetaExportOOo" ) );
}

uno::Reference< uno::XInterface > SAL_CALL XMLMetaExportOOO_createInstance(
		const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
	throw( uno::Exception )
{
	// #110680#
	// return (cppu::OWeakObject*)new XMLMetaExportComponent;
	return (cppu::OWeakObject*)new XMLMetaExportComponent(rSMgr, EXPORT_META);
}

