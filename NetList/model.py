
from component import Component

#from model_f24x import F24X

def Model(board, sexp):
    ''' Find the right (sub)class or this component '''
    part = sexp.find_first("libsource.part")[0].name
    cls = {
        #'"F240"': F24X,
        #'"F244"': F24X,
    }.get(part, Component)
    return cls(board, sexp)
