//
// GraphPath.g Parser for the Graph Path Language.
//

grammar GraphPath;

path: subgraphpath? (GT DELIMITER contained_path)? ;

subgraphpath
    :   subgraph // opt_delimited_relative_path
    |   subgraph DELIMITER subgraphpath
    ;

subgraph: LBRACKET REGULAR RBRACKET;

contained_path
    :   vertex // opt_delimited_relative_path
    |   vertex edge // opt_delimited_relative_path
    |   vertex edge contained_path
    ;

vertex: LT REGULAR GT (LBRACE REGULAR RBRACE REGULAR?)? ;
edge: DELIMITER REGULAR DELIMITER GT ;

//relative_continuation: /* empty */
//      |   subgraphpath
//      |   contained_path
//      ;
//
//opt_delimited_relative_path: /* empty */
//      |   RELATIVE_GRAPHPATH relative_continuation
//      ;

DELIMITER:	'--' ;
LBRACKET: 	'[' ;
RBRACKET: 	']' ;
LBRACE: 	'}' ;
RBRACE: 	'{' ;
LT: 		'<' ;
GT: 		'>' ;
TEMPRALREL:	'<<' ;
STRUCTREL:	':>' ;
REGULAR: [0-9A-Za-z+/=.;\'"]+ ;
WS: [ \t\r\n]+ -> skip ;
COMMENT: ( '//' ~[\r\n]* '\r'? '\n' | '/*' .*? '*/') -> channel(HIDDEN) ;
