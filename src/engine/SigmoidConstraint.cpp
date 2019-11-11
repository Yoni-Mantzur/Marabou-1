//
// Created by yoni_mantzur on 5/18/19.
//

#include "SigmoidConstraint.h"
#include "ConstraintBoundTightener.h"
#include "Debug.h"
#include "FloatUtils.h"
#include "ITableau.h"
#include "InputQuery.h"
#include "MStringf.h"
#include "PiecewiseLinearCaseSplit.h"
#include "MarabouError.h"
#include "Statistics.h"
#include "TableauRow.h"

#ifdef _WIN32
#define __attribute__(x)
#endif

SigmoidConstraint::SigmoidConstraint( unsigned b, unsigned f )
        : _b ( b )
        , _f( f )
{
}

SigmoidConstraint::SigmoidConstraint( const String &serializedSigmoid )
{
    String constraintType = serializedSigmoid.substring(0, 7);
    ASSERT(constraintType == String("sigmoid"));

    // remove the constraint type in serialized form
    String serializedValues = serializedSigmoid.substring(5, serializedSigmoid.length()-7);
    List<String> values = serializedValues.tokenize( "," );
    _b = atoi( values.back().ascii() );
    _f = atoi( values.front().ascii() );
}

PiecewiseLinearConstraint *SigmoidConstraint::duplicateConstraint() const
{
    SigmoidConstraint *clone = new SigmoidConstraint( _b, _f );
    *clone = *this;
    return clone;
}


void SigmoidConstraint::restoreState( const PiecewiseLinearConstraint *state )
{
    const auto *sigmoid = dynamic_cast<const SigmoidConstraint *>( state );
    *this = *sigmoid;
}

void SigmoidConstraint::registerAsWatcher( ITableau *tableau )
{
    tableau->registerToWatchVariable( this, _b );
    tableau->registerToWatchVariable( this, _f );
}

void SigmoidConstraint::unregisterAsWatcher( ITableau *tableau )
{
    tableau->unregisterToWatchVariable( this, _b );
    tableau->unregisterToWatchVariable( this, _f );
}

void SigmoidConstraint::notifyVariableValue( unsigned variable, double value )
{
    if ( FloatUtils::isZero( 1 - value, GlobalConfiguration::SIGMOID_CONSTRAINT_COMPARISON_TOLERANCE ) )
        value = 0.0;

    if ( FloatUtils::isZero( 1 + value, GlobalConfiguration::SIGMOID_CONSTRAINT_COMPARISON_TOLERANCE ) )
        value = 0.0;

    _assignment[variable] = value;
}

void SigmoidConstraint::notifyLowerBound( unsigned variable, double bound )
{
    if ( _statistics )
        _statistics->incNumBoundNotificationsPlConstraints();

    if ( _lowerBounds.exists( variable ) && !FloatUtils::gt( bound, _lowerBounds[variable] ) )
        return;

    _lowerBounds[variable] = bound;

    double sigmoidBound = FloatUtils::sigmoid(bound);
    if (variable == _b && FloatUtils::gt(sigmoidBound, _lowerBounds[_f] ) ) {
        _constraintBoundTightener->registerTighterLowerBound(_f, sigmoidBound);
        return;
    }

    double sigmoidInverseBound = FloatUtils::sigmoidInverse(bound);
    if (variable == _f && FloatUtils::gt(sigmoidInverseBound, _lowerBounds[_b] ) )
        _constraintBoundTightener->registerTighterLowerBound(_b, sigmoidInverseBound);
}

void SigmoidConstraint::notifyUpperBound( unsigned variable, double bound )
{
    if ( _statistics )
        _statistics->incNumBoundNotificationsPlConstraints();

    if ( _upperBounds.exists( variable ) && !FloatUtils::lt( bound, _upperBounds[variable] ) )
        return;

    _upperBounds[variable] = bound;

    double sigmoidBound = FloatUtils::sigmoid(bound);
    if (variable == _b && FloatUtils::lt(sigmoidBound, _upperBounds[_f] ) ) {
        _constraintBoundTightener->registerTighterUpperBound(_f, sigmoidBound);
        return;
    }

    double sigmoidInverseBound = FloatUtils::sigmoidInverse(bound);
    if (variable == _f && FloatUtils::lt(sigmoidInverseBound, _upperBounds[_b] ) )
        _constraintBoundTightener->registerTighterUpperBound(_b, sigmoidInverseBound);
}

bool SigmoidConstraint::participatingVariable( unsigned variable ) const
{
    return ( variable == _b ||  variable == _f );
}

List<unsigned> SigmoidConstraint::getParticipatingVariables() const
{
    List<unsigned> participatingVariables;

    participatingVariables.append( _b );
    participatingVariables.append( _f );

    return participatingVariables;
}

bool SigmoidConstraint::satisfied() const
{
    if ( !( _assignment.exists( _b ) && _assignment.exists( _f ) ) )
        throw MarabouError( MarabouError::PARTICIPATING_VARIABLES_ABSENT );

    double bValue = _assignment.get( _b );
    double fValue = _assignment.get( _f );

    return FloatUtils::areEqual( FloatUtils::sigmoid(bValue), fValue,
            GlobalConfiguration::SIGMOID_CONSTRAINT_COMPARISON_TOLERANCE );
}

List<PiecewiseLinearConstraint::Fix> SigmoidConstraint::getPossibleFixes() const
{
    ASSERT(!satisfied());
    ASSERT(_assignment.exists(_b));
    ASSERT(_assignment.exists(_f));

    double bValue = _assignment.get(_b);
    double sigmoidValue = FloatUtils::sigmoid(bValue);

    double fValue = _assignment.get(_f);
    double sigmoidInverseValue = FloatUtils::sigmoidInverse(fValue);

    List <PiecewiseLinearConstraint::Fix> fixes;

    fixes.append(Fix(_f, sigmoidValue ));
    fixes.append(Fix(_b, sigmoidInverseValue ));
    return fixes;
}

List<PiecewiseLinearConstraint::Fix> SigmoidConstraint::getSmartFixes(__attribute__((unused)) ITableau *tableau ) const
{
    // TODO: write something smarter
    return getPossibleFixes();
}

List<PiecewiseLinearCaseSplit> SigmoidConstraint::getCaseSplits() const
{
    ASSERT(_assignment.exists(_b));
    ASSERT(_assignment.exists(_f));
    ASSERT(_lowerBounds.exists(_b) && _lowerBounds.exists(_f));
    ASSERT(_upperBounds.exists(_b) && _upperBounds.exists(_f));

    double bValue = _assignment.get(_b);
    double sigmoidValue = FloatUtils::sigmoid(bValue);

    // TODO: Refactor this casting
    // TODO: guided points should be append in satisfied
    List<GuidedPoint> guidedPoints;
    guidedPoints.append(GuidedPoint(_lowerBounds[_b], _lowerBounds[_f]));
    guidedPoints.append(GuidedPoint(bValue, sigmoidValue));
    guidedPoints.append(GuidedPoint(_upperBounds[_b], _upperBounds[_f]));

    List<PiecewiseLinearCaseSplit> splits = getRefinedSplits(guidedPoints);
    // TODO: ADD UPPER EQUATIONS

    return splits;
}

void SigmoidConstraint::dump( String &output ) const
{
    output = Stringf( "SigmoidConstraint: x%u = sigmoid( x%u ).\n", _f, _b);

    output += Stringf( "b in [%s, %s], ",
                       _lowerBounds.exists( _b ) ? Stringf( "%lf", _lowerBounds[_b] ).ascii() : "-inf",
                       _upperBounds.exists( _b ) ? Stringf( "%lf", _upperBounds[_b] ).ascii() : "inf" );

    output += Stringf( "f in [%s, %s]",
                       _lowerBounds.exists( _f ) ? Stringf( "%lf", _lowerBounds[_f] ).ascii() : "-inf",
                       _upperBounds.exists( _f ) ? Stringf( "%lf", _upperBounds[_f] ).ascii() : "inf" );
}

void SigmoidConstraint::updateVariableIndex( unsigned oldIndex, unsigned newIndex )
{
    ASSERT( oldIndex == _b || oldIndex == _f );
    ASSERT( !_assignment.exists( newIndex ) &&
            !_lowerBounds.exists( newIndex ) &&
            !_upperBounds.exists( newIndex ) &&
            newIndex != _b && newIndex != _f );

    if ( _assignment.exists( oldIndex ) )
    {
        _assignment[newIndex] = _assignment.get( oldIndex );
        _assignment.erase( oldIndex );
    }

    if ( _lowerBounds.exists( oldIndex ) )
    {
        _lowerBounds[newIndex] = _lowerBounds.get( oldIndex );
        _lowerBounds.erase( oldIndex );
    }

    if ( _upperBounds.exists( oldIndex ) )
    {
        _upperBounds[newIndex] = _upperBounds.get( oldIndex );
        _upperBounds.erase( oldIndex );
    }

    if ( oldIndex == _b )
        _b = newIndex;
    else
        _f = newIndex;
}

void SigmoidConstraint::eliminateVariable( __attribute__((unused)) unsigned variable,
                                        __attribute__((unused)) double fixedValue )
{
    //TODO: implement this
}

bool SigmoidConstraint::constraintObsolete() const
{
    //TODO: implement this
    return false;
}

void SigmoidConstraint::getEntailedTightenings(List<Tightening> &tightenings ) const
{
    ASSERT( _lowerBounds.exists( _b ) && _lowerBounds.exists( _f ) &&
            _upperBounds.exists( _b ) && _upperBounds.exists( _f ) );

    double bLowerBound = _lowerBounds[_b], sigmoidbLowerBound = FloatUtils::sigmoid( bLowerBound );
    double fLowerBound = _lowerBounds[_f], sigmoidInversefLowerBound = FloatUtils::sigmoidInverse( fLowerBound );

    double bUpperBound = _upperBounds[_b], sigmoidbUpperBound = FloatUtils::sigmoid( bUpperBound );
    double fUpperBound = _upperBounds[_f], sigmoidInversefUpperBound = FloatUtils::sigmoidInverse( fUpperBound );

    if (FloatUtils::lt(bLowerBound, sigmoidInversefLowerBound))
        tightenings.append(Tightening(_b, sigmoidInversefLowerBound, Tightening::LB));
    
    if (FloatUtils::lt(fLowerBound, sigmoidbLowerBound))
        tightenings.append(Tightening(_f, sigmoidbLowerBound, Tightening::LB));
    
    if (FloatUtils::gt(bUpperBound, sigmoidInversefUpperBound))
        tightenings.append(Tightening(_b, sigmoidInversefUpperBound, Tightening::UB));

    if (FloatUtils::gt(fUpperBound, sigmoidbUpperBound))
        tightenings.append(Tightening(_f, sigmoidbUpperBound, Tightening::UB));
}

void SigmoidConstraint::addAuxiliaryEquations(__attribute__((unused)) InputQuery &inputQuery )
{
    //TODO: implement this
}

void SigmoidConstraint::getCostFunctionComponent(__attribute__((unused)) Map<unsigned, double> &cost ) const
{
    //TODO: implement this
}

String SigmoidConstraint::serializeToString() const
{
    // Output format is: sigmoid,f,b
    return Stringf( "sigmoid,%u,%u", _f, _b );
}

bool SigmoidConstraint::supportsSymbolicBoundTightening() const
{
    //TODO: implement this
    return false;
}


