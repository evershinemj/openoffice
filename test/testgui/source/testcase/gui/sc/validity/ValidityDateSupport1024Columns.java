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

package testcase.gui.sc.validity;

import static org.junit.Assert.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.io.File;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;
import org.openoffice.test.vcl.IDList;
import org.openoffice.test.vcl.widgets.VclMessageBox;

import testlib.gui.SCTool;

public class ValidityDateSupport1024Columns {
	private static IDList idList = new IDList(new File("./ids"));
	public static final VclMessageBox ActiveMsgBox = new VclMessageBox(idList.getId("UID_ACTIVE"), "Message on message box.");

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);
	}

	@After
	public void tearDown() throws Exception {

	}

	/**
	 * test Allow Greater than or equal to Date type in Validity, support 1024
	 * columns.
	 */
	@Test
	public void testValidityDateSupport1024Columns() {
		app.dispatch("private:factory/scalc");
		SCTool.selectRange("ALM1000:ALO1005");
		app.dispatch(".uno:Validation");

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(3); // "Date"
		scValidityDecimalCompareOperator.select(2); // "greater than"
		scValiditySourceInput.setText("01/01/08");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("ALM1001");
		scInputBarInput.activate();
		typeKeys("02/01/08<enter>");
		assertEquals("02/01/08", SCTool.getCellText("ALM1001"));

		SCTool.selectRange("ALM1002");
		scInputBarInput.activate();
		typeKeys("01/02/08<enter>");
		assertEquals("01/02/08", SCTool.getCellText("ALM1002"));

		SCTool.selectRange("ALM1003");
		scInputBarInput.activate();
		typeKeys("01/01/08<enter>");
		assertEquals("Invalid value", ActiveMsgBox.getMessage());
		ActiveMsgBox.ok();
		assertEquals("", SCTool.getCellText("ALM1003"));

		SCTool.selectRange("AML1003");
		scInputBarInput.activate();
		typeKeys("12/31/07<enter>");
		assertEquals("Invalid value", ActiveMsgBox.getMessage());
		ActiveMsgBox.ok();
		assertEquals("", SCTool.getCellText("AML1003"));
	}
}
