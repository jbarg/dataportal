//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser.impl;

public class CollectionListImpl implements edu.sdsc.matrix.srb.parser.CollectionList, com.sun.xml.bind.unmarshaller.UnmarshallableObject, com.sun.xml.bind.serializer.XMLSerializable, com.sun.xml.bind.validator.ValidatableObject
{

    protected com.sun.xml.bind.util.ListImpl _DataSets = new com.sun.xml.bind.util.ListImpl(new java.util.ArrayList());
    protected com.sun.xml.bind.util.ListImpl _Collections = new com.sun.xml.bind.util.ListImpl(new java.util.ArrayList());
    protected com.sun.xml.bind.util.ListImpl _CollectionsExtendedInfo = new com.sun.xml.bind.util.ListImpl(new java.util.ArrayList());
    private final static com.sun.msv.grammar.Grammar schemaFragment = com.sun.xml.bind.validator.SchemaDeserializer.deserialize("\u00ac\u00ed\u0000\u0005sr\u0000\u001fcom.sun.msv.grammar.SequenceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.BinaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0004exp1t\u0000 Lcom/sun/msv/grammar/Expression;L\u0000\u0004exp2q\u0000~\u0000\u0002xr\u0000\u001ecom.sun.msv.grammar.Expression\u00f8\u0018\u0082\u00e8N5~O\u0002\u0000\u0003I\u0000\u000ecachedHashCodeL\u0000\u0013epsilonReducibilityt\u0000\u0013Ljava/lang/Boolean;L\u0000\u000bexpandedExpq\u0000~\u0000\u0002xp\u0005o(\u00b3ppsr\u0000\u001dcom.sun.msv.grammar.ChoiceExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0001\u0003\u009fpqppsq\u0000~\u0000\u0006\u0001\u00cf\u00b8=ppsr\u0000 com.sun.msv.grammar.OneOrMoreExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001ccom.sun.msv.grammar.UnaryExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\u0003expq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b82sr\u0000\u0011java.lang.Boolean\u00cd r\u0080\u00d5\u009c\u00fa\u00ee\u0002\u0000\u0001Z\u0000\u0005valuexp\u0000psr\u0000\'com.sun.msv.grammar.trex.ElementPattern\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\tnameClasst\u0000\u001fLcom/sun/msv/grammar/NameClass;xr\u0000\u001ecom.sun.msv.grammar.ElementExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002Z\u0000\u001aignoreUndeclaredAttributesL\u0000\fcontentModelq\u0000~\u0000\u0002xq\u0000~\u0000\u0003\u0001\u00cf\u00b8/q\u0000~\u0000\rp\u0000sq\u0000~\u0000\u000e\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\t\u0001\u00cf\u00b8\u000eq\u0000~\u0000\rpsr\u0000 com.sun.msv.grammar.AttributeExp\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\u0003expq\u0000~\u0000\u0002L\u0000\tnameClassq\u0000~\u0000\u000fxq\u0000~\u0000\u0003\u0001\u00cf\u00b8\u000bq\u0000~\u0000\rpsr\u00002com.sun.msv.grammar.Expression$AnyStringExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\bsq\u0000~\u0000\f\u0001q\u0000~\u0000\u0018sr\u0000 com.sun.msv.grammar.AnyNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xr\u0000\u001dcom.sun.msv.grammar.NameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xpsr\u00000com.sun.msv.grammar.Expression$EpsilonExpression\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0000xq\u0000~\u0000\u0003\u0000\u0000\u0000\tq\u0000~\u0000\u0019psr\u0000#com.sun.msv.grammar.SimpleNameClass\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0002L\u0000\tlocalNamet\u0000\u0012Ljava/lang/String;L\u0000\fnamespaceURIq\u0000~\u0000 xq\u0000~\u0000\u001bt\u0000)edu.sdsc.matrix.srb.parser.CollectionListt\u0000+http://java.sun.com/jaxb/xjc/dummy-elementssq\u0000~\u0000\u001ft\u0000\u0017CollectionsExtendedInfot\u0000(http://www.teragrid.org/schemas/datagridq\u0000~\u0000\u001esq\u0000~\u0000\t\u0001\u00cf\u00b82ppsq\u0000~\u0000\u000e\u0001\u00cf\u00b8/pp\u0000sq\u0000~\u0000\u000e\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\t\u0001\u00cf\u00b8\u000eq\u0000~\u0000\rpsq\u0000~\u0000\u0015\u0001\u00cf\u00b8\u000bq\u0000~\u0000\rpq\u0000~\u0000\u0018q\u0000~\u0000\u001cq\u0000~\u0000\u001esq\u0000~\u0000\u001ft\u0000%edu.sdsc.matrix.srb.parser.Collectionq\u0000~\u0000#sq\u0000~\u0000\u001ft\u0000\u000bCollectionsq\u0000~\u0000&sq\u0000~\u0000\u0006\u0001\u00cf\u00b8=ppsq\u0000~\u0000\t\u0001\u00cf\u00b82q\u0000~\u0000\rpsq\u0000~\u0000\u000e\u0001\u00cf\u00b8/q\u0000~\u0000\rp\u0000sq\u0000~\u0000\u000e\u0001\u00cf\u00b8$pp\u0000sq\u0000~\u0000\u0006\u0001\u00cf\u00b8\u0019ppsq\u0000~\u0000\t\u0001\u00cf\u00b8\u000eq\u0000~\u0000\rpsq\u0000~\u0000\u0015\u0001\u00cf\u00b8\u000bq\u0000~\u0000\rpq\u0000~\u0000\u0018q\u0000~\u0000\u001cq\u0000~\u0000\u001esq\u0000~\u0000\u001ft\u0000.edu.sdsc.matrix.srb.parser.DataSetExtendedListq\u0000~\u0000#sq\u0000~\u0000\u001ft\u0000\bdataSetsq\u0000~\u0000&q\u0000~\u0000\u001esr\u0000\"com.sun.msv.grammar.ExpressionPool\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0001\u0002\u0000\u0001L\u0000\bexpTablet\u0000/Lcom/sun/msv/grammar/ExpressionPool$ClosedHash;xpsr\u0000-com.sun.msv.grammar.ExpressionPool$ClosedHash\u00d7j\u00d0N\u00ef\u00e8\u00ed\u001c\u0002\u0000\u0004I\u0000\u0005countI\u0000\tthresholdL\u0000\u0006parentq\u0000~\u0000=[\u0000\u0005tablet\u0000![Lcom/sun/msv/grammar/Expression;xp\u0000\u0000\u0000\r\u0000\u0000\u00009pur\u0000![Lcom.sun.msv.grammar.Expression;\u00d68D\u00c3]\u00ad\u00a7\n\u0002\u0000\u0000xp\u0000\u0000\u0000\u00bfppppppppppq\u0000~\u0000\u0005pppppppppppppppppppppppppppppppppppppppppppppppppppppq\u0000~\u0000\u0007ppppppppppppppppppppq\u0000~\u0000\u0014q\u0000~\u0000+q\u0000~\u00006ppppppppq\u0000~\u0000\u0013q\u0000~\u0000*q\u0000~\u00005ppppppppppppppppppppppq\u0000~\u0000\u000bq\u0000~\u0000\'q\u0000~\u00002ppppppppq\u0000~\u0000\bq\u0000~\u00001ppppppppppppppppppppppppppppppppppppppppppppppppppppppppp");

    private final static java.lang.Class PRIMARY_INTERFACE_CLASS() {
        return edu.sdsc.matrix.srb.parser.CollectionList.class;
    }

    public java.util.List getDataSets() {
        return _DataSets;
    }

    public java.util.List getCollections() {
        return _Collections;
    }

    public java.util.List getCollectionsExtendedInfo() {
        return _CollectionsExtendedInfo;
    }

    public com.sun.xml.bind.unmarshaller.ContentHandlerEx getUnmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
        return new edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.Unmarshaller(context);
    }

    public java.lang.Class getPrimaryInterfaceClass() {
        return PRIMARY_INTERFACE_CLASS();
    }

    public void serializeElements(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        int idx1 = 0;
        final int len1 = _DataSets.size();
        int idx2 = 0;
        final int len2 = _Collections.size();
        int idx3 = 0;
        final int len3 = _CollectionsExtendedInfo.size();
        if ((_CollectionsExtendedInfo.size()>= 1)&&(_Collections.size() == 0)) {
            while (idx3 != len3) {
                if (_CollectionsExtendedInfo.get(idx3) instanceof javax.xml.bind.Element) {
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _CollectionsExtendedInfo.get(idx3 ++)));
                } else {
                    context.startElement("http://www.teragrid.org/schemas/datagrid", "CollectionsExtendedInfo");
                    int idx_0 = idx3;
                    context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _CollectionsExtendedInfo.get(idx_0 ++)));
                    context.endAttributes();
                    context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _CollectionsExtendedInfo.get(idx3 ++)));
                    context.endElement();
                }
            }
        } else {
            if ((_CollectionsExtendedInfo.size() == 0)&&(_Collections.size()>= 1)) {
                while (idx2 != len2) {
                    if (_Collections.get(idx2) instanceof javax.xml.bind.Element) {
                        context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Collections.get(idx2 ++)));
                    } else {
                        context.startElement("http://www.teragrid.org/schemas/datagrid", "Collections");
                        int idx_1 = idx2;
                        context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _Collections.get(idx_1 ++)));
                        context.endAttributes();
                        context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _Collections.get(idx2 ++)));
                        context.endElement();
                    }
                }
            }
        }
        while (idx1 != len1) {
            if (_DataSets.get(idx1) instanceof javax.xml.bind.Element) {
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _DataSets.get(idx1 ++)));
            } else {
                context.startElement("http://www.teragrid.org/schemas/datagrid", "dataSets");
                int idx_2 = idx1;
                context.childAsAttributes(((com.sun.xml.bind.serializer.XMLSerializable) _DataSets.get(idx_2 ++)));
                context.endAttributes();
                context.childAsElements(((com.sun.xml.bind.serializer.XMLSerializable) _DataSets.get(idx1 ++)));
                context.endElement();
            }
        }
    }

    public void serializeAttributes(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        int idx1 = 0;
        final int len1 = _DataSets.size();
        int idx2 = 0;
        final int len2 = _Collections.size();
        int idx3 = 0;
        final int len3 = _CollectionsExtendedInfo.size();
    }

    public void serializeAttributeBodies(com.sun.xml.bind.serializer.XMLSerializer context)
        throws org.xml.sax.SAXException
    {
        int idx1 = 0;
        final int len1 = _DataSets.size();
        int idx2 = 0;
        final int len2 = _Collections.size();
        int idx3 = 0;
        final int len3 = _CollectionsExtendedInfo.size();
    }

    public java.lang.Class getPrimaryInterface() {
        return (edu.sdsc.matrix.srb.parser.CollectionList.class);
    }

    public com.sun.msv.verifier.DocumentDeclaration createRawValidator() {
        return new com.sun.msv.verifier.regexp.REDocumentDeclaration(schemaFragment);
    }

    public class Unmarshaller
        extends com.sun.xml.bind.unmarshaller.ContentHandlerEx
    {


        public Unmarshaller(com.sun.xml.bind.unmarshaller.UnmarshallingContext context) {
            super(context, "-------");
        }

        protected com.sun.xml.bind.unmarshaller.UnmarshallableObject owner() {
            return edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.this;
        }

        public void enterElement(java.lang.String ___uri, java.lang.String ___local, org.xml.sax.Attributes __atts)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  1 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("AnyDirectory" == ___local)) {
                        _Collections.add(((edu.sdsc.matrix.srb.parser.impl.CollectionImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.CollectionImpl.class), 2, ___uri, ___local, __atts)));
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("StdCollection" == ___local)) {
                        _Collections.add(((edu.sdsc.matrix.srb.parser.impl.CollectionImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.CollectionImpl.class), 2, ___uri, ___local, __atts)));
                        return ;
                    }
                    break;
                case  3 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("DataSet" == ___local)) {
                        _DataSets.add(((edu.sdsc.matrix.srb.parser.impl.DataSetExtendedListImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.DataSetExtendedListImpl.class), 4, ___uri, ___local, __atts)));
                        return ;
                    }
                    break;
                case  5 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("dataSets" == ___local)) {
                        _CollectionsExtendedInfo.add(((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.class), 6, ___uri, ___local, __atts)));
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("Collections" == ___local)) {
                        _CollectionsExtendedInfo.add(((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.class), 6, ___uri, ___local, __atts)));
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("CollectionsExtendedInfo" == ___local)) {
                        _CollectionsExtendedInfo.add(((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl) spawnChildFromEnterElement((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.class), 6, ___uri, ___local, __atts)));
                        return ;
                    }
                    break;
                case  0 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("dataSets" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 3;
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("Collections" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 1;
                        return ;
                    }
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("CollectionsExtendedInfo" == ___local)) {
                        context.pushAttributes(__atts);
                        state = 5;
                        return ;
                    }
                    revertToParentFromEnterElement(___uri, ___local, __atts);
                    return ;
            }
            super.enterElement(___uri, ___local, __atts);
        }

        public void leaveElement(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  2 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("Collections" == ___local)) {
                        context.popAttributes();
                        state = 0;
                        return ;
                    }
                    break;
                case  4 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("dataSets" == ___local)) {
                        context.popAttributes();
                        state = 0;
                        return ;
                    }
                    break;
                case  6 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("CollectionsExtendedInfo" == ___local)) {
                        context.popAttributes();
                        state = 0;
                        return ;
                    }
                    break;
                case  5 :
                    if (("http://www.teragrid.org/schemas/datagrid" == ___uri)&&("CollectionsExtendedInfo" == ___local)) {
                        _CollectionsExtendedInfo.add(((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl) spawnChildFromLeaveElement((edu.sdsc.matrix.srb.parser.impl.CollectionListImpl.class), 6, ___uri, ___local)));
                        return ;
                    }
                    break;
                case  0 :
                    revertToParentFromLeaveElement(___uri, ___local);
                    return ;
            }
            super.leaveElement(___uri, ___local);
        }

        public void enterAttribute(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  0 :
                    revertToParentFromEnterAttribute(___uri, ___local);
                    return ;
            }
            super.enterAttribute(___uri, ___local);
        }

        public void leaveAttribute(java.lang.String ___uri, java.lang.String ___local)
            throws com.sun.xml.bind.unmarshaller.UnreportedException
        {
            switch (state) {
                case  0 :
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
                    case  0 :
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
                case  2 :
                    state = 2;
                    return ;
                case  4 :
                    state = 4;
                    return ;
                case  6 :
                    state = 6;
                    return ;
            }
            super.leaveChild(nextState);
        }

    }

}
