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



#ifndef _SVX_UNOEDHLP_HXX
#define _SVX_UNOEDHLP_HXX

#include <memory>
#include <tools/solar.h>
#include <svtools/textdata.hxx>
#include <svl/hint.hxx>
#include <tools/gen.hxx> 
#include "editeng/editengdllapi.h"

struct EENotify;
class EditEngine;

#define EDITSOURCE_HINT_PARASMOVED			20
#define EDITSOURCE_HINT_SELECTIONCHANGED	21

/** Extends TextHint by two additional parameters which are necessary
    for the EDITSOURCE_HINT_PARASMOVED hint. TextHint's value in this
    case denotes the destination position, the two parameters the
    start and the end of the moved paragraph range.
 */
class EDITENG_DLLPUBLIC SvxEditSourceHint : public TextHint
{
private:
	sal_uLong 	mnStart;
	sal_uLong 	mnEnd;

public:
			TYPEINFO();
			SvxEditSourceHint( sal_uLong nId );
			SvxEditSourceHint( sal_uLong nId, sal_uLong nValue, sal_uLong nStart=0, sal_uLong nEnd=0 );

	sal_uLong 	GetValue() const;
    sal_uLong	GetStartValue() const;
    sal_uLong	GetEndValue() const;
	void	SetValue( sal_uLong n );
	void	SetStartValue( sal_uLong n );
	void	SetEndValue( sal_uLong n );
};
//IAccessibility2 Implementation 2009-----
class SvxEditSourceHintEndPara :public SvxEditSourceHint
{
public:
	TYPEINFO();
	SvxEditSourceHintEndPara( sal_uInt32 nId )
		:SvxEditSourceHint(nId) {}
	SvxEditSourceHintEndPara( sal_uInt32 nId, sal_uInt32 nValue, sal_uInt32 nStart=0, sal_uInt32 nEnd=0 )
		:SvxEditSourceHint(nId,nValue,nStart){ (void)nEnd; }
};
//-----IAccessibility2 Implementation 2009
/** Helper class for common functionality in edit sources
 */
class EDITENG_DLLPUBLIC SvxEditSourceHelper
{
public:

    /** Translates EditEngine notifications into broadcastable hints

    	@param aNotify
        Notification object send by the EditEngine.

        @return the translated hint
     */
    static ::std::auto_ptr<SfxHint> EENotification2Hint( EENotify* aNotify );

    /** Calculate attribute run for EditEngines

    	Please note that the range returned is half-open: [nStartIndex,nEndIndex)

    	@param nStartIndex 
        Herein, the start index of the range of similar attributes is returned

    	@param nEndIndex 
        Herein, the end index (exclusive) of the range of similar attributes is returned

        @param rEE
        The EditEngine to query for attributes

        @param nPara
        The paragraph the following index value is to be interpreted in

        @param nIndex
        The character index from which the range of similar attributed characters is requested

        @return sal_True, if the range has been successfully determined
     */
	//IAccessibility2 Implementation 2009-----
    //static sal_Bool GetAttributeRun( USHORT& nStartIndex, USHORT& nEndIndex, const EditEngine& rEE, USHORT nPara, USHORT nIndex );
    static sal_Bool GetAttributeRun( sal_uInt16& nStartIndex, sal_uInt16& nEndIndex, const EditEngine& rEE, sal_uInt16 nPara, sal_uInt16 nIndex, sal_Bool bInCell=sal_False );
	//-----IAccessibility2 Implementation 2009

    /** Convert point from edit engine to user coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given point appropriately,
    	if vertical writing is on.

        @param rPoint
        Point to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed point
     */
    static Point EEToUserSpace( const Point& rPoint, const Size& rEESize, bool bIsVertical );

    /** Convert point from user to edit engine coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given point appropriately,
    	if vertical writing is on.

        @param rPoint
        Point to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed point
     */
    static Point UserSpaceToEE( const Point& rPoint, const Size& rEESize, bool bIsVertical );

    /** Convert rect from edit engine to user coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given rect appropriately,
    	if vertical writing is on.

        @param rRect
        Rectangle to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed rect
     */
    static Rectangle EEToUserSpace( const Rectangle& rRect, const Size& rEESize, bool bIsVertical );

    /** Convert rect from user to edit engine coordinate space

    	As the edit engine internally keeps vertical text unrotated,
    	all internal edit engine methods return their stuff unrotated,
    	too. This method rotates and shifts given rect appropriately,
    	if vertical writing is on.

        @param rRect
        Rectangle to transform

        @param rEESize
        Paper size of the edit engine

        @param  bIsVertical
        Whether output text is vertical or not

        @return the possibly transformed rect
     */
    static Rectangle UserSpaceToEE( const Rectangle& rRect, const Size& rEESize, bool bIsVertical );

};

#endif
