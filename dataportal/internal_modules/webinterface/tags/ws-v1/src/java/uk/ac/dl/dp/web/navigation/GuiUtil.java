/*
 * Copyright 2004-2006 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package uk.ac.dl.dp.web.navigation;

import javax.faces.context.FacesContext;
import java.util.ResourceBundle;
import java.util.MissingResourceException;
import java.text.MessageFormat;

/**
 * @author Thomas Spiegl (latest modification by $Author: baranda $)
 * @version $Revision: 368646 $ $Date: 2006-01-13 09:08:20 +0000 (Fri, 13 Jan 2006) $
 */
public class GuiUtil
{
    private static String BUNDLE_NAME = "uk.example_messages";

    public static String getMessageResource(String key, Object[] arguments)
    {
        FacesContext context = FacesContext.getCurrentInstance();
        String resourceString;
        try
        {
            ResourceBundle bundle = ResourceBundle.getBundle(BUNDLE_NAME, context.getViewRoot().getLocale());
            resourceString = bundle.getString(key);
        }
        catch (MissingResourceException e)
        {
            return key;
        }

        if (arguments == null) return resourceString;

        MessageFormat format = new MessageFormat(resourceString, context.getViewRoot().getLocale());
        return format.format(arguments);
    }

}
