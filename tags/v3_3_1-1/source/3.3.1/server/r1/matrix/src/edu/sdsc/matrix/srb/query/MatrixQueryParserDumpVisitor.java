package edu.sdsc.matrix.srb.query;


/* Developer log
	* Original code  Reena Mathew
	* Created developer log, removed the unnecessary sun's licence 1/31/05
	*/

public class MatrixQueryParserDumpVisitor implements MatrixQueryParserVisitor {
	private int indent = 0;
	private String indentString() {
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < indent; ++i) {
			sb.append(" ");
		}
		return sb.toString();
	}

	public Object visit(SimpleNode node, Object data) {
		System.out.println(indentString() + node +
									": acceptor not unimplemented in subclass?");
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
	}

	public Object visit(ASTStart node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
	}

	/*
			public Object visit(ASTAdd node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
			}
			public Object visit(ASTMult node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
			}
		*/
	public Object visit(ASTMyOtherID node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
	}

	/*
			public Object visit(ASTInteger node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
			}
		*/
	public Object visit(ASTNotSupported node, Object data) {
		System.out.println(indentString() + node);
		++indent;
		data = node.childrenAccept(this, data);
		--indent;
		return data;
	}
}
/*end*/
