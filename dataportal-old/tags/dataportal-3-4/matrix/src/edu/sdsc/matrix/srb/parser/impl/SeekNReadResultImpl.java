//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser.impl;

public class SeekNReadResultImpl implements edu.sdsc.matrix.srb.parser.SeekNReadResult, com.sun.xml.bind.unmarshaller.UnmarshallableObject, com.sun.xml.bind.serializer.XMLSerializable, com.sun.xml.bind.validator.ValidatableObject
{

    protected edu.sdsc.matrix.srb.parser.AnyType _ExternalURl;
    protected edu.sdsc.matrix.srb.parser.AnyType _Comments;
    protected edu.sdsc.matrix.srb.parser.AnyType _ContentId;
    private final static com.sun.msv.grammar.Grammar schemaFragment = com.sun.xml.bind.validator.SchemaDeserializer.deserialize("\u00ac\u00ed\u0000\u0005sr\u0000\u001fcom.sun.msv.grammar.SequenceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.BinaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0004exp1t\u0000 Lcom/sun/msv/grammar/Expression;L\u0000\u0004exp2q\u0000~\u0000\u0002xr\u0000\u001ecom.sun.msv.grammar.Expression\u00f8\u0018\u0082\u00e8N5~O\u0002\u0000\u0003I\u0000\u000ecachedHashCodeL\u0000\u0013epsilonReducibilityt\u0000\u0013Ljava/lang/Boolean;L\u0000\u000bexpandedExpq\u0000~\u0000\u0002xp\u0005o(\u0094ppsr\u0000\u001dcom.sun.msv.grammar.ChoiceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0001\u0003\u009fp`ppsr\u0000\'com.sun.msv.grammar.trex.ElementPattern\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\tnameClasst\u0000\u001fLcom/sun/msv/grammar/NameClass;xr\u0000\u001ecom.sun.msv.grammar.ElementExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002Z\u0000\u001aignoreUndeclaredAttributesL\u0000\fcontentModelq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\b\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsr\u0000 com.sun.msv.grammar.OneOrMoreExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001ccom.sun.msv.grammar.UnaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\u0003expq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000esr\u0000\u0011java.lang.Boolean\u00cd r\u0080\u00d5\u009c\u00fa\u00ee\u0002\u0000\u0001Z\u0000\u0005valuexp\u0000psr\u0000 com.sun.msv.grammar.AttributeExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0003expq\u0000~\u0000\u0002L\u0000\tnameClassq\u0000~\u0000\txq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0012psr\u00002com.sun.msv.grammar.Expression$AnyStringExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\bsq\u0000~\u0000\u0011\u0001q\u0000~\u0000\u0016sr\u0000 com.sun.msv.grammar.AnyNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.NameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xpsr\u00000com.sun.msv.grammar.Expression$EpsilonExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\tq\u0000~\u0000\u0017psr\u0000#com.sun.msv.grammar.SimpleNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\tlocalNamet\u0000\u0012Ljava/lang/String;L\u0000\fnamespaceURIq\u0000~\u0000\u001exq\u0000~\u0000\u0019t\u0000\"edu.sdsc.matrix.srb.parser.AnyTypet\u0000+http://java.sun.com/jaxb/xjc/dummy-elementssq\u0000~\u0000\u001dt\u0000\tcontentIdt\u0000(http://www.teragrid.org/schemas/datagridsq\u0000~\u0000\b\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\b\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\u000e\u0001\u00cf\u00b8\u000eq\u0000~\u0000\u0012psq\u0000~\u0000\u0013\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0012pq\u0000~\u0000\u0016q\u0000~\u0000\u001aq\u0000~\u0000\u001csq\u0000~\u0000\u001dq\u0000~\u0000 q\u0000~\u0000!sq\u0000~\u0000\u001dt\u0000\u000bexternalURlq\u0000~\u0000$sq\u0000~\u0000\b\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\b\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\u000e\u0001\u00cf\u00b8\u000eq\u0000~\u0000\u0012psq\u0000~\u0000\u0013\u0001\u00cf\u00b8\u000bq\u0000~\u0000\u0012pq\u0000~\u0000\u0016q\u0000~\u0000\u001aq\u0000~\u0000\u001csq\u0000~\u0000\u001dq\u0000~\u0000 q\u0000~\u0000!sq\u0000~\u0000\u001dt\u0000\bcommentsq\u0000~\u0000$sr\u0000\"com.sun.msv.grammar.ExpressionPool\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\bexpTablet\u0000/Lcom/sun/msv/grammar/ExpressionPool$ClosedHash;xpsr\u0000-com.sun.msv.grammar.ExpressionPool$ClosedHash\u00d7j\u00d0N\u00ef\u00e8\u00ed\u001c\u0002\u0000\u0004I\u0000\u0005countI\u0000\tthresholdL\u0000\u0006parentq\u0000~\u00006[\u0000\u0005tablet\u0000![Lcom/sun/msv/grammar/Expression;xp\u0000\u0000\u0000\b\u0000\u0000\u00009pur\u0000![Lcom.sun.msv.grammar.Expression;\u00d68D\u00c3]\u00ad\u00a7\n\u0002\u0000\u0000xp\u0000\u0000\u0000\u00bfpppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0007pppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0010q\u0000~\u0000(q\u0000~\u00000ppppppppq\u0000~\u0000\rq\u0000~\u0000\'q\u0000~\u0000/pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0005pppppppppppppppppppp");

    private final static java.lang.Class PRIMARY_INTERFACE_CLASS() {
        return edu.sdsc.matrix.srb.parser.SeekNReadResult.class;
    }

    public edu.sdsc.matrix.srb.parser.AnyType getExternalURl() {
        return _ExternalURl;
    }

    public void setExternalURl(edu.sdsc.matrix.srb.parser.AnyType value) {
        _ExternalURl = value;
    }

    public edu.sdsc.matrix.srb.parser.AnyType getComments() {
        return _Comments;
    }

    public void setComments(edu.sdsc.matrix.srb.parser.AnyType value) {
        _Comments = value;
    }

    public edu.sdsc.matrix.srb.parser.AnyType getContentId() {
        return _ContentId;
    }

    public void setContentId(edu.sdsc.matrix.srb.parser.AnyType value) {
        _ContentId = value;
    }

    public com.sun.xml.bind.unmarshaller.ContentHandlerEx getUnmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
        return new edu.sdsc.matrix.srb.parser.impl.SeekNReadResultImpl.Unmarshaller(context);
    }

    public java.lang.Class getPrimaryInterfaceClass() {
        return PRIMARY_INTERFACE_CLASS();
    }

    public void serializeElements(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        if ((_ExternalURl == null)&&(_ContentId!= null)) {
            if (_ContentId instanceof javax.xml.bind.Element) {
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _ContentId));
            } else {
                context.startElement("http://www.teragrid.org/schemas/datagrid", "contentId");
                context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _ContentId));
                context.endAttributes();
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _ContentId));
                context.endElement();
            }
        } else {
            if ((_ExternalURl!= null)&&(_ContentId == null)) {
                if (_ExternalURl instanceof javax.xml.bind.Element) {
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _ExternalURl));
                } else {
                    context.startElement("http://www.teragrid.org/schemas/datagrid", "externalURl");
                    context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _ExternalURl));
                    context.endAttributes();
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _ExternalURl));
                    context.endElement();
                }
            }
        }
        if (_Comments instanceof javax.xml.bind.Element) {
            context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Comments));
        } else {
            context.startElement("http://www.teragrid.org/schemas/datagrid", "comments");
            context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _Comments));
            context.endAttributes();
            context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Comments));
            context.endElement();
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
        return (edu.sdsc.matrix.srb.parser.SeekNReadResult.class);
    }

    public com.sun.msv.verifier.DocumentDeclaration createRawValidator() {
        return new com.sun.msv.verifier.regexp.REDocumentDeclaration(schemaFragment);
    }

    public class Unmarshaller
        extends com.sun.xml.bind.unmarshaller.ContentHandlerEx
    {


        public Unmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
            super(context, "---------");
        }

        protected com.sun.xml.bind.unmarshaller.UnmarshallableObject owner() {
            return edu.sdsc.matrix.srb.parser.impl.SeekNReadResultImpl.this;
        }

        public void enterElement(java.lang.String ___uri, java.lang.String ___local, org.xml.sax.Attributes __atts)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  7 :
                    if (true) {
                        _ExternalURl = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 8, ___uri, ___local, __atts));
                        return ;
                    }
                    break;
                case  0 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("externalURl" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 7;
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("contentId" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 1;
                        return ;
                    }
                    break;
                case  6 :
                    revertToParentFromEnterElement(___uri, ___local, __atts);
                    return ;
                case  3 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("comments" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 4;
                        return ;
                    }
                    break;
                case  4 :
                    if (true) {
                        _Comments = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 5, ___uri, ___local, __atts));
                        return ;
                    }
                    break;
                case  1 :
                    if (true) {
                        _ContentId = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 2, ___uri, ___local, __atts));
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
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("comments" == ___local)) {
                        context.popAttributes();
                        state = 6;
                        return ;
                    }
                    break;
                case  7 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("externalURl" == ___local)) {
                        _ExternalURl = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromLeaveElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 8, ___uri, ___local));
                        return ;
                    }
                    break;
                case  6 :
                    revertToParentFromLeaveElement(___uri, ___local);
                    return ;
                case  8 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("externalURl" == ___local)) {
                        context.popAttributes();
                        state = 3;
                        return ;
                    }
                    break;
                case  2 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("contentId" == ___local)) {
                        context.popAttributes();
                        state = 3;
                        return ;
                    }
                    break;
                case  4 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("comments" == ___local)) {
                        _Comments = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromLeaveElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 5, ___uri, ___local));
                        return ;
                    }
                    break;
                case  1 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("contentId" == ___local)) {
                        _ContentId = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromLeaveElement((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 2, ___uri, ___local));
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
                case  6 :
                    revertToParentFromEnterAttribute(___uri, ___local);
                    return ;
            }
            super.enterAttribute(___uri, ___local);
        }

        public void leaveAttribute(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  6 :
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
                    case  7 :
                        _ExternalURl = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromText((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 8, value));
                        return ;
                    case  6 :
                        revertToParentFromText(value);
                        return ;
                    case  4 :
                        _Comments = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromText((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 5, value));
                        return ;
                    case  1 :
                        _ContentId = ((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl) spawnChildFromText((edu.sdsc.matrix.srb.parser.impl.AnyTypeImpl.class), 2, value));
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
                case  8 :
                    state = 8;
                    return ;
                case  2 :
                    state = 2;
                    return ;
            }
            super.leaveChild(nextState);
        }

    }

}