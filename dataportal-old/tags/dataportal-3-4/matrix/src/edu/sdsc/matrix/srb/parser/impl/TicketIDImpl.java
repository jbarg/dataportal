//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser.impl;

public class TicketIDImpl implements edu.sdsc.matrix.srb.parser.TicketID, com.sun.xml.bind.unmarshaller.UnmarshallableObject, com.sun.xml.bind.serializer.XMLSerializable, com.sun.xml.bind.validator.ValidatableObject
{

    protected edu.sdsc.matrix.srb.parser.Key _Key;
    protected edu.sdsc.matrix.srb.parser.TicketID.TicketStringType _TicketString;
    private final static com.sun.msv.grammar.Grammar schemaFragment = com.sun.xml.bind.validator.SchemaDeserializer.deserialize("\u00ac\u00ed\u0000\u0005sr\u0000\u001dcom.sun.msv.grammar.ChoiceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.BinaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0004exp1t\u0000 Lcom/sun/msv/grammar/Expression;L\u0000\u0004exp2q\u0000~\u0000\u0002xr\u0000\u001ecom.sun.msv.grammar.Expression\u00f8\u0018\u0082\u00e8N5~O\u0002\u0000\u0003I\u0000\u000ecachedHashCodeL\u0000\u0013epsilonReducibilityt\u0000\u0013Ljava/lang/Boolean;L\u0000\u000bexpandedExpq\u0000~\u0000\u0002xp\u0003\u009fp`ppsr\u0000\'com.sun.msv.grammar.trex.ElementPattern\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\tnameClasst\u0000\u001fLcom/sun/msv/grammar/NameClass;xr\u0000\u001ecom.sun.msv.grammar.ElementExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002Z\u0000\u001aignoreUndeclaredAttributesL\u0000\fcontentModelq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0000\u0001\u00cf\u00b8\u0019ppsr\u0000 com.sun.msv.grammar.OneOrMoreExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001ccom.sun.msv.grammar.UnaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\u0003expq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000esr\u0000\u0011java.lang.Boolean\u00cd r\u0080\u00d5\u009c\u00fa\u00ee\u0002\u0000\u0001Z\u0000\u0005valuexp\u0000psr\u0000 com.sun.msv.grammar.AttributeExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0003expq\u0000~\u0000\u0002L\u0000\tnameClassq\u0000~\u0000\u0007xq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0010psr\u00002com.sun.msv.grammar.Expression$AnyStringExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\bsq\u0000~\u0000\u000f\u0001q\u0000~\u0000\u0014sr\u0000 com.sun.msv.grammar.AnyNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.NameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xpsr\u00000com.sun.msv.grammar.Expression$EpsilonExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\tq\u0000~\u0000\u0015psr\u0000#com.sun.msv.grammar.SimpleNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\tlocalNamet\u0000\u0012Ljava/lang/String;L\u0000\fnamespaceURIq\u0000~\u0000\u001cxq\u0000~\u0000\u0017t\u0000\u001eedu.sdsc.matrix.srb.parser.Keyt\u0000+http://java.sun.com/jaxb/xjc/dummy-elementssq\u0000~\u0000\u001bt\u0000\u0003keyt\u0000(http://www.teragrid.org/schemas/datagridsq\u0000~\u0000\u0006\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0000\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\f\u0001\u00cf\u00b8\u000eq\u0000~\u0000\u0010psq\u0000~\u0000\u0011\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0010pq\u0000~\u0000\u0014q\u0000~\u0000\u0018q\u0000~\u0000\u001asq\u0000~\u0000\u001bt\u00004edu.sdsc.matrix.srb.parser.TicketID.TicketStringTypeq\u0000~\u0000\u001fsq\u0000~\u0000\u001bt\u0000\fticketStringq\u0000~\u0000\"sr\u0000\"com.sun.msv.grammar.ExpressionPool\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\bexpTablet\u0000/Lcom/sun/msv/grammar/ExpressionPool$ClosedHash;xpsr\u0000-com.sun.msv.grammar.ExpressionPool$ClosedHash\u00d7j\u00d0N\u00ef\u00e8\u00ed\u001c\u0002\u0000\u0004I\u0000\u0005countI\u0000\tthresholdL\u0000\u0006parentq\u0000~\u0000-[\u0000\u0005tablet\u0000![Lcom/sun/msv/grammar/Expression;xp\u0000\u0000\u0000\u0005\u0000\u0000\u00009pur\u0000![Lcom.sun.msv.grammar.Expression;\u00d68D\u00c3]\u00ad\u00a7\n\u0002\u0000\u0000xp\u0000\u0000\u0000\u00bfpppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0005pppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u000eq\u0000~\u0000&pppppppppq\u0000~\u0000\u000bq\u0000~\u0000%ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp");

    private final static java.lang.Class PRIMARY_INTERFACE_CLASS() {
        return edu.sdsc.matrix.srb.parser.TicketID.class;
    }

    public edu.sdsc.matrix.srb.parser.Key getKey() {
        return _Key;
    }

    public void setKey(edu.sdsc.matrix.srb.parser.Key value) {
        _Key = value;
    }

    public edu.sdsc.matrix.srb.parser.TicketID.TicketStringType getTicketString() {
        return _TicketString;
    }

    public void setTicketString(edu.sdsc.matrix.srb.parser.TicketID.TicketStringType value) {
        _TicketString = value;
    }

    public com.sun.xml.bind.unmarshaller.ContentHandlerEx getUnmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
        return new edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.Unmarshaller(context);
    }

    public java.lang.Class getPrimaryInterfaceClass() {
        return PRIMARY_INTERFACE_CLASS();
    }

    public void serializeElements(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        if ((_Key!= null)&&(_TicketString == null)) {
            if (_Key instanceof javax.xml.bind.Element) {
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Key));
            } else {
                context.startElement("http://www.teragrid.org/schemas/datagrid", "key");
                context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _Key));
                context.endAttributes();
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Key));
                context.endElement();
            }
        } else {
            if ((_Key == null)&&(_TicketString!= null)) {
                if (_TicketString instanceof javax.xml.bind.Element) {
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _TicketString));
                } else {
                    context.startElement("http://www.teragrid.org/schemas/datagrid", "ticketString");
                    context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _TicketString));
                    context.endAttributes();
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _TicketString));
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
        return (edu.sdsc.matrix.srb.parser.TicketID.class);
    }

    public com.sun.msv.verifier.DocumentDeclaration createRawValidator() {
        return new com.sun.msv.verifier.regexp.REDocumentDeclaration(schemaFragment);
    }

    public static class TicketStringTypeImpl implements edu.sdsc.matrix.srb.parser.TicketID.TicketStringType, com.sun.xml.bind.unmarshaller.UnmarshallableObject, com.sun.xml.bind.serializer.XMLSerializable, com.sun.xml.bind.validator.ValidatableObject
    {

        protected java.lang.String _Value;
        protected java.lang.String _TicketType;
        private final static com.sun.msv.grammar.Grammar schemaFragment = com.sun.xml.bind.validator.SchemaDeserializer.deserialize("\u00ac\u00ed\u0000\u0005sr\u0000\u001fcom.sun.msv.grammar.SequenceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.BinaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0004exp1t\u0000 Lcom/sun/msv/grammar/Expression;L\u0000\u0004exp2q\u0000~\u0000\u0002xr\u0000\u001ecom.sun.msv.grammar.Expression\u00f8\u0018\u0082\u00e8N5~O\u0002\u0000\u0003I\u0000\u000ecachedHashCodeL\u0000\u0013epsilonReducibilityt\u0000\u0013Ljava/lang/Boolean;L\u0000\u000bexpandedExpq\u0000~\u0000\u0002xp\u0004E\u0083mppsr\u0000\u001bcom.sun.msv.grammar.DataExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0003L\u0000\u0002dtt\u0000\u001fLorg/relaxng/datatype/Datatype;L\u0000\u0006exceptq\u0000~\u0000\u0002L\u0000\u0004namet\u0000\u001dLcom/sun/msv/util/StringPair;xq\u0000~\u0000\u0003\u0001\u00b6\u008d\u0088sr\u0000\u0011java.lang.Boolean\u00cd r\u0080\u00d5\u009c\u00fa\u00ee\u0002\u0000\u0001Z\u0000\u0005valuexp\u0000psr\u0000#com.sun.msv.datatype.xsd.StringType\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001Z\u0000\risAlwaysValidxr\u0000*com.sun.msv.datatype.xsd.BuiltinAtomicType\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000%com.sun.msv.datatype.xsd.ConcreteType\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\'com.sun.msv.datatype.xsd.XSDatatypeImpl\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0003L\u0000\fnamespaceUrit\u0000\u0012Ljava/lang/String;L\u0000\btypeNameq\u0000~\u0000\u0010L\u0000\nwhiteSpacet\u0000.Lcom/sun/msv/datatype/xsd/WhiteSpaceProcessor;xpt\u0000 http://www.w3.org/2001/XMLSchemat\u0000\u0006stringsr\u0000.com.sun.msv.datatype.xsd.WhiteSpaceProcessor$1\u0013JMoI\u00db\u00a4G\u0002\u0000\u0000xr\u0000,com.sun.msv.datatype.xsd.WhiteSpaceProcessor\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xp\u0001sr\u00000com.sun.msv.grammar.Expression$NullSetExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\nq\u0000~\u0000\u000bpsr\u0000\u001bcom.sun.msv.util.StringPair\u00d0t\u001ejB\u008f\u008d\u00a0\u0002\u0000\u0002L\u0000\tlocalNameq\u0000~\u0000\u0010L\u0000\fnamespaceURIq\u0000~\u0000\u0010xpq\u0000~\u0000\u0014q\u0000~\u0000\u0013sr\u0000\u001dcom.sun.msv.grammar.ChoiceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0001\u0002\u008e\u00f5\u00e0ppsr\u0000 com.sun.msv.grammar.AttributeExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0003expq\u0000~\u0000\u0002L\u0000\tnameClasst\u0000\u001fLcom/sun/msv/grammar/NameClass;xq\u0000~\u0000\u0003\u0002\u008e\u00f5\u00d5q\u0000~\u0000\u000bpsq\u0000~\u0000\u0006\u0001\u00d5k\u00ceppsr\u0000)com.sun.msv.datatype.xsd.EnumerationFacet\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\u0006valuest\u0000\u000fLjava/util/Set;xr\u00009com.sun.msv.datatype.xsd.DataTypeWithValueConstraintFacet\"\u00a7Ro\u00ca\u00c7\u008aT\u0002\u0000\u0000xr\u0000*com.sun.msv.datatype.xsd.DataTypeWithFacet\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0005Z\u0000\fisFacetFixedZ\u0000\u0012needValueCheckFlagL\u0000\bbaseTypet\u0000)Lcom/sun/msv/datatype/xsd/XSDatatypeImpl;L\u0000\fconcreteTypet\u0000\'Lcom/sun/msv/datatype/xsd/ConcreteType;L\u0000\tfacetNameq\u0000~\u0000\u0010xq\u0000~\u0000\u000ft\u0000\u0000pq\u0000~\u0000\u0017\u0000\u0000q\u0000~\u0000\u0012q\u0000~\u0000\u0012t\u0000\u000benumerationsr\u0000\u0011java.util.HashSet\u00baD\u0085\u0095\u0096\u00b8\u00b74\u0003\u0000\u0000xpw\f\u0000\u0000\u0000\u0010?@\u0000\u0000\u0000\u0000\u0000\u0002t\u0000\tSRBTickett\u0000\u0005Otherxq\u0000~\u0000\u0019sq\u0000~\u0000\u001at\u0000\u000estring-derivedq\u0000~\u0000)sr\u0000#com.sun.msv.grammar.SimpleNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\tlocalNameq\u0000~\u0000\u0010L\u0000\fnamespaceURIq\u0000~\u0000\u0010xr\u0000\u001dcom.sun.msv.grammar.NameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xpt\u0000\nticketTypeq\u0000~\u0000)sr\u00000com.sun.msv.grammar.Expression$EpsilonExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\tsq\u0000~\u0000\n\u0001psr\u0000\"com.sun.msv.grammar.ExpressionPool\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\bexpTablet\u0000/Lcom/sun/msv/grammar/ExpressionPool$ClosedHash;xpsr\u0000-com.sun.msv.grammar.ExpressionPool$ClosedHash\u00d7j\u00d0N\u00ef\u00e8\u00ed\u001c\u0002\u0000\u0004I\u0000\u0005countI\u0000\tthresholdL\u0000\u0006parentq\u0000~\u00009[\u0000\u0005tablet\u0000![Lcom/sun/msv/grammar/Expression;xp\u0000\u0000\u0000\u0002\u0000\u0000\u00009pur\u0000![Lcom.sun.msv.grammar.Expression;\u00d68D\u00c3]\u00ad\u00a7\n\u0002\u0000\u0000xp\u0000\u0000\u0000\u00bfppppppppppppppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u001dppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0005ppppppppppppppppppppppppppppppppppppppppppp");

        private final static java.lang.Class PRIMARY_INTERFACE_CLASS() {
            return edu.sdsc.matrix.srb.parser.TicketID.TicketStringType.class;
        }

        public java.lang.String getValue() {
            return _Value;
        }

        public void setValue(java.lang.String value) {
            _Value = value;
        }

        public java.lang.String getTicketType() {
            return _TicketType;
        }

        public void setTicketType(java.lang.String value) {
            _TicketType = value;
        }

        public com.sun.xml.bind.unmarshaller.ContentHandlerEx getUnmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
            return new edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl.Unmarshaller(context);
        }

        public java.lang.Class getPrimaryInterfaceClass() {
            return PRIMARY_INTERFACE_CLASS();
        }

        public void serializeElements(com.sun.xml.bind.serializer.XMLSerializer context)
            throws org.xml.sax.SAXException
        {
            try {
                context.text(((java.lang.String) _Value));
            } catch (java.lang.Exception e) {
                com.sun.xml.bind.marshaller.Util.handlePrintConversionException(this, e, context);
            }
        }

        public void serializeAttributes(com.sun.xml.bind.serializer.XMLSerializer context)
            throws org.xml.sax.SAXException
        {
            if (_TicketType!= null) {
                context.startAttribute("", "ticketType");
                try {
                    context.text(((java.lang.String) _TicketType));
                } catch (java.lang.Exception e) {
                    com.sun.xml.bind.marshaller.Util.handlePrintConversionException(this, e, context);
                }
                context.endAttribute();
            }
        }

        public void serializeAttributeBodies(com.sun.xml.bind.serializer.XMLSerializer context)
            throws org.xml.sax.SAXException
        {
            try {
                context.text(((java.lang.String) _Value));
            } catch (java.lang.Exception e) {
                com.sun.xml.bind.marshaller.Util.handlePrintConversionException(this, e, context);
            }
        }

        public java.lang.Class getPrimaryInterface() {
            return (edu.sdsc.matrix.srb.parser.TicketID.TicketStringType.class);
        }

        public com.sun.msv.verifier.DocumentDeclaration createRawValidator() {
            return new com.sun.msv.verifier.regexp.REDocumentDeclaration(schemaFragment);
        }

        public class Unmarshaller
            extends com.sun.xml.bind.unmarshaller.ContentHandlerEx
        {


            public Unmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
                super(context, "----");
            }

            protected com.sun.xml.bind.unmarshaller.UnmarshallableObject owner() {
                return edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl.this;
            }

            public void enterElement(java.lang.String ___uri, java.lang.String ___local, org.xml.sax.Attributes __atts)
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                switch (state) {
                    case  1 :
                        revertToParentFromEnterElement(___uri, ___local, __atts);
                        return ;
                }
                super.enterElement(___uri, ___local, __atts);
            }

            public void leaveElement(java.lang.String ___uri, java.lang.String ___local)
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                switch (state) {
                    case  1 :
                        revertToParentFromLeaveElement(___uri, ___local);
                        return ;
                }
                super.leaveElement(___uri, ___local);
            }

            public void enterAttribute(java.lang.String ___uri, java.lang.String ___local)
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                switch (state) {
                    case  1 :
                        revertToParentFromEnterAttribute(___uri, ___local);
                        return ;
                    case  0 :
                        if (("" == ___uri)&&("ticketType" == ___local)) {
                            state = 2;
                            return ;
                        }
                        break;
                }
                super.enterAttribute(___uri, ___local);
            }

            public void leaveAttribute(java.lang.String ___uri, java.lang.String ___local)
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                switch (state) {
                    case  1 :
                        revertToParentFromLeaveAttribute(___uri, ___local);
                        return ;
                    case  3 :
                        if (("" == ___uri)&&("ticketType" == ___local)) {
                            goto0();
                            return ;
                        }
                        break;
                }
                super.leaveAttribute(___uri, ___local);
            }

            public void text(java.lang.String value)
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                try {
                    switch (state) {
                        case  2 :
                            try {
                                _TicketType = value;
                            } catch (java.lang.Exception e) {
                                handleParseConversionException(e);
                            }
                            state = 3;
                            return ;
                        case  1 :
                            revertToParentFromText(value);
                            return ;
                        case  0 :
                            try {
                                _Value = value;
                            } catch (java.lang.Exception e) {
                                handleParseConversionException(e);
                            }
                            state = 1;
                            return ;
                    }
                } catch (java.lang.RuntimeException e) {
                    handleUnexpectedTextException(value, e);
                }
            }

            private void goto0()
                throws com.sun.xml.bind.unmarshaller.UnreportedException
            {
                int idx;
                state = 0;
                idx = context.getAttribute("", "ticketType");
                if (idx >= 0) {
                    context.consumeAttribute(idx);
                    return ;
                }
            }

        }

    }

    public class Unmarshaller
        extends com.sun.xml.bind.unmarshaller.ContentHandlerEx
    {


        public Unmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
            super(context, "------");
        }

        protected com.sun.xml.bind.unmarshaller.UnmarshallableObject owner() {
            return edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.this;
        }

        public void enterElement(java.lang.String ___uri, java.lang.String ___local, org.xml.sax.Attributes __atts)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  3 :
                    revertToParentFromEnterElement(___uri, ___local, __atts);
                    return ;
                case  1 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("keyValue" == ___local)) {
                        _Key = ((edu.sdsc.matrix.srb.parser.impl.KeyImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.KeyImpl.class), 2, ___uri, ___local, __atts));
                        return ;
                    }
                    break;
                case  0 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("ticketString" == ___local)) {
                        context.pushAttributes(__atts);
                        goto4();
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("key" == ___local)) {
                        context.pushAttributes(__atts);
                        goto1();
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
                case  3 :
                    revertToParentFromLeaveElement(___uri, ___local);
                    return ;
                case  2 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("key" == ___local)) {
                        context.popAttributes();
                        state = 3;
                        return ;
                    }
                    break;
                case  5 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("ticketString" == ___local)) {
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
                case  4 :
                    if (("" == ___uri)&&("ticketType" == ___local)) {
                        _TicketString = ((edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl) spawnChildFromEnterAttribute((edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl.class), 5, ___uri, ___local));
                        return ;
                    }
                    break;
                case  1 :
                    if (("" == ___uri)&&("keyType" == ___local)) {
                        _Key = ((edu.sdsc.matrix.srb.parser.impl.KeyImpl) spawnChildFromEnterAttribute((edu.sdsc.matrix.srb.parser.impl.KeyImpl.class), 2, ___uri, ___local));
                        return ;
                    }
                    if (("" == ___uri)&&("keyVersion" == ___local)) {
                        _Key = ((edu.sdsc.matrix.srb.parser.impl.KeyImpl) spawnChildFromEnterAttribute((edu.sdsc.matrix.srb.parser.impl.KeyImpl.class), 2, ___uri, ___local));
                        return ;
                    }
                    break;
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
                    case  4 :
                        _TicketString = ((edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl) spawnChildFromText((edu.sdsc.matrix.srb.parser.impl.TicketIDImpl.TicketStringTypeImpl.class), 5, value));
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
                case  2 :
                    state = 2;
                    return ;
                case  5 :
                    state = 5;
                    return ;
            }
            super.leaveChild(nextState);
        }

        private void goto4()
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            int idx;
            state = 4;
            idx = context.getAttribute("", "ticketType");
            if (idx >= 0) {
                context.consumeAttribute(idx);
                return ;
            }
        }

        private void goto1()
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            int idx;
            state = 1;
            idx = context.getAttribute("", "keyVersion");
            if (idx >= 0) {
                context.consumeAttribute(idx);
                return ;
            }
            idx = context.getAttribute("", "keyType");
            if (idx >= 0) {
                context.consumeAttribute(idx);
                return ;
            }
        }

    }

}