//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser.impl;

public class ReplicateOpImpl implements edu.sdsc.matrix.srb.parser.ReplicateOp, com.sun.xml.bind.unmarshaller.UnmarshallableObject, com.sun.xml.bind.serializer.XMLSerializable, com.sun.xml.bind.validator.ValidatableObject
{

    protected edu.sdsc.matrix.srb.parser.ParamList _FlexParams;
    protected edu.sdsc.matrix.srb.parser.ReplicateParams _StdParams;
    private final static com.sun.msv.grammar.Grammar schemaFragment = com.sun.xml.bind.validator.SchemaDeserializer.deserialize("\u00ac\u00ed\u0000\u0005sr\u0000\u001dcom.sun.msv.grammar.ChoiceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.BinaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0004exp1t\u0000 Lcom/sun/msv/grammar/Expression;L\u0000\u0004exp2q\u0000~\u0000\u0002xr\u0000\u001ecom.sun.msv.grammar.Expression\u00f8\u0018\u0082\u00e8N5~O\u0002\u0000\u0003I\u0000\u000ecachedHashCodeL\u0000\u0013epsilonReducibilityt\u0000\u0013Ljava/lang/Boolean;L\u0000\u000bexpandedExpq\u0000~\u0000\u0002xp\u0003\u009fp`ppsr\u0000\'com.sun.msv.grammar.trex.ElementPattern\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\tnameClasst\u0000\u001fLcom/sun/msv/grammar/NameClass;xr\u0000\u001ecom.sun.msv.grammar.ElementExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002Z\u0000\u001aignoreUndeclaredAttributesL\u0000\fcontentModelq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0000\u0001\u00cf\u00b8\u0019ppsr\u0000 com.sun.msv.grammar.OneOrMoreExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001ccom.sun.msv.grammar.UnaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\u0003expq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000esr\u0000\u0011java.lang.Boolean\u00cd r\u0080\u00d5\u009c\u00fa\u00ee\u0002\u0000\u0001Z\u0000\u0005valuexp\u0000psr\u0000 com.sun.msv.grammar.AttributeExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0003expq\u0000~\u0000\u0002L\u0000\tnameClassq\u0000~\u0000\u0007xq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0010psr\u00002com.sun.msv.grammar.Expression$AnyStringExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\bsq\u0000~\u0000\u000f\u0001q\u0000~\u0000\u0014sr\u0000 com.sun.msv.grammar.AnyNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.NameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xpsr\u00000com.sun.msv.grammar.Expression$EpsilonExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\tq\u0000~\u0000\u0015psr\u0000#com.sun.msv.grammar.SimpleNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\tlocalNamet\u0000\u0012Ljava/lang/String;L\u0000\fnamespaceURIq\u0000~\u0000\u001cxq\u0000~\u0000\u0017t\u0000*edu.sdsc.matrix.srb.parser.ReplicateParamst\u0000+http://java.sun.com/jaxb/xjc/dummy-elementssq\u0000~\u0000\u001bt\u0000\tStdParamst\u0000(http://www.teragrid.org/schemas/datagridsq\u0000~\u0000\u0006\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0000\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\f\u0001\u00cf\u00b8\u000eq\u0000~\u0000\u0010psq\u0000~\u0000\u0011\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0010pq\u0000~\u0000\u0014q\u0000~\u0000\u0018q\u0000~\u0000\u001asq\u0000~\u0000\u001bt\u0000$edu.sdsc.matrix.srb.parser.ParamListq\u0000~\u0000\u001fsq\u0000~\u0000\u001bt\u0000\nFlexParamsq\u0000~\u0000\"sr\u0000\"com.sun.msv.grammar.ExpressionPool\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\bexpTablet\u0000/Lcom/sun/msv/grammar/ExpressionPool$ClosedHash;xpsr\u0000-com.sun.msv.grammar.ExpressionPool$ClosedHash\u00d7j\u00d0N\u00ef\u00e8\u00ed\u001c\u0002\u0000\u0004I\u0000\u0005countI\u0000\tthresholdL\u0000\u0006parentq\u0000~\u0000-[\u0000\u0005tablet\u0000![Lcom/sun/msv/grammar/Expression;xp\u0000\u0000\u0000\u0005\u0000\u0000\u00009pur\u0000![Lcom.sun.msv.grammar.Expression;\u00d68D\u00c3]\u00ad\u00a7\n\u0002\u0000\u0000xp\u0000\u0000\u0000\u00bfpppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0005pppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u000eq\u0000~\u0000&pppppppppq\u0000~\u0000\u000bq\u0000~\u0000%ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp");

    private final static java.lang.Class PRIMARY_INTERFACE_CLASS() {
        return edu.sdsc.matrix.srb.parser.ReplicateOp.class;
    }

    public edu.sdsc.matrix.srb.parser.ParamList getFlexParams() {
        return _FlexParams;
    }

    public void setFlexParams(edu.sdsc.matrix.srb.parser.ParamList value) {
        _FlexParams = value;
    }

    public edu.sdsc.matrix.srb.parser.ReplicateParams getStdParams() {
        return _StdParams;
    }

    public void setStdParams(edu.sdsc.matrix.srb.parser.ReplicateParams value) {
        _StdParams = value;
    }

    public com.sun.xml.bind.unmarshaller.ContentHandlerEx getUnmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
        return new edu.sdsc.matrix.srb.parser.impl.ReplicateOpImpl.Unmarshaller(context);
    }

    public java.lang.Class getPrimaryInterfaceClass() {
        return PRIMARY_INTERFACE_CLASS();
    }

    public void serializeElements(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        if ((_StdParams!= null)&&(_FlexParams == null)) {
            if (_StdParams instanceof javax.xml.bind.Element) {
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _StdParams));
            } else {
                context.startElement("http://www.teragrid.org/schemas/datagrid", "StdParams");
                context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _StdParams));
                context.endAttributes();
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _StdParams));
                context.endElement();
            }
        } else {
            if ((_StdParams == null)&&(_FlexParams!= null)) {
                if (_FlexParams instanceof javax.xml.bind.Element) {
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _FlexParams));
                } else {
                    context.startElement("http://www.teragrid.org/schemas/datagrid", "FlexParams");
                    context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _FlexParams));
                    context.endAttributes();
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _FlexParams));
                    context.endElement();
                }
            }
        }
    }

    public void serializeAttributes(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
    }

    public void serializeAttributeBodies(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
    }

    public java.lang.Class getPrimaryInterface() {
        return (edu.sdsc.matrix.srb.parser.ReplicateOp.class);
    }

    public com.sun.msv.verifier.DocumentDeclaration createRawValidator() {
        return new com.sun.msv.verifier.regexp.REDocumentDeclaration(schemaFragment);
    }

    public class Unmarshaller
        extends com.sun.xml.bind.unmarshaller.ContentHandlerEx
    {


        public Unmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
            super(context, "------");
        }

        protected com.sun.xml.bind.unmarshaller.UnmarshallableObject owner() {
            return edu.sdsc.matrix.srb.parser.impl.ReplicateOpImpl.this;
        }

        public void enterElement(java.lang.String ___uri, java.lang.String ___local, org.xml.sax.Attributes __atts)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  0 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("FlexParams" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 1;
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("StdParams" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 4;
                        return ;
                    }
                    break;
                case  4 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("DataSource" == ___local)) {
                        _StdParams = ((edu.sdsc.matrix.srb.parser.impl.ReplicateParamsImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.ReplicateParamsImpl.class), 5, ___uri, ___local, __atts));
                        return ;
                    }
                    break;
                case  3 :
                    revertToParentFromEnterElement(___uri, ___local, __atts);
                    return ;
                case  1 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("param" == ___local)) {
                        _FlexParams = ((edu.sdsc.matrix.srb.parser.impl.ParamListImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.ParamListImpl.class), 2, ___uri, ___local, __atts));
                        return ;
                    }
                    break;
            }
            super.enterElement(___uri, ___local, __atts);
        }

        public void leaveElement(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  5 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("StdParams" == ___local)) {
                        context.popAttributes();
                        state = 3;
                        return ;
                    }
                    break;
                case  3 :
                    revertToParentFromLeaveElement(___uri, ___local);
                    return ;
                case  1 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("FlexParams" == ___local)) {
                        _FlexParams = ((edu.sdsc.matrix.srb.parser.impl.ParamListImpl) spawnChildFromLeaveElement((edu.sdsc.matrix.srb.parser.impl.ParamListImpl.class), 2, ___uri, ___local));
                        return ;
                    }
                    break;
                case  2 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("FlexParams" == ___local)) {
                        context.popAttributes();
                        state = 3;
                        return ;
                    }
                    break;
            }
            super.leaveElement(___uri, ___local);
        }

        public void enterAttribute(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  3 :
                    revertToParentFromEnterAttribute(___uri, ___local);
                    return ;
            }
            super.enterAttribute(___uri, ___local);
        }

        public void leaveAttribute(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  3 :
                    revertToParentFromLeaveAttribute(___uri, ___local);
                    return ;
            }
            super.leaveAttribute(___uri, ___local);
        }

        public void text(java.lang.String value)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            try {
                switch (state) {
                    case  3 :
                        revertToParentFromText(value);
                        return ;
                }
            } catch (java.lang.RuntimeException e) {
                handleUnexpectedTextException(value, e);
            }
        }

        public void leaveChild(int nextState)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (nextState) {
                case  5 :
                    state = 5;
                    return ;
                case  2 :
                    state = 2;
                    return ;
            }
            super.leaveChild(nextState);
        }

    }

}
