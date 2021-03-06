//
// Created by yoni_mantzur on 11/3/19.
//

#ifndef MARABOU_SIGMOIDCONSTRAINT_H
#define MARABOU_SIGMOIDCONSTRAINT_H

#include <File.h>
#include "Map.h"
#include "PiecewiseLinearConstraint.h"
#include "PiecewiseLinearAbstraction.h"

class SigmoidConstraint : public PiecewiseLinearConstraint, PiecewiseLinearAbstraction
{
public:
    SigmoidConstraint( unsigned b, unsigned f );
    explicit SigmoidConstraint( const String &serializedSigmoid );

    /*
      Get the type of this constraint.
    */
    PiecewiseLinearFunctionType getType() const override { return  SIGMOID; };

    /*
      Return a clone of the constraint.
    */
    PiecewiseLinearConstraint *duplicateConstraint() const override;

    /*
      Restore the state of this constraint from the given one.
    */
    void restoreState( const PiecewiseLinearConstraint *state ) override;

    /*
      Register/unregister the constraint with a talbeau.
     */
    void registerAsWatcher( ITableau *tableau ) override;
    void unregisterAsWatcher( ITableau *tableau ) override;

    /*
      These callbacks are invoked when a watched variable's value
      changes, or when its bounds change.
    */
    void notifyVariableValue( unsigned variable, double value ) override;
    void notifyLowerBound( unsigned variable, double bound ) override;
    void notifyUpperBound( unsigned variable, double bound ) override;

    /*
     * Notification when broken assignment is invoked
     */
    void notifyBrokenAssignment() override;

    /*
      Returns true iff the variable participates in this piecewise
      linear constraint
    */
    bool participatingVariable( unsigned variable ) const override;

    /*
      Get the list of variables participating in this constraint.
    */
    List<unsigned> getParticipatingVariables() const override;

    /*
      Returns true iff the assignment satisfies the constraint
    */
    bool satisfied() const override;

    /*
      Returns a list of possible fixes for the violated constraint.
    */
    List<PiecewiseLinearConstraint::Fix> getPossibleFixes() const override;

    /*
      Return a list of smart fixes for violated constraint.
    */
    List<PiecewiseLinearConstraint::Fix> getSmartFixes( ITableau *tableau ) const override;

    /*
      Returns the list of case splits that this piecewise linear
      constraint breaks into. These splits need to complementary,
      i.e. if the list is {l1, l2, ..., ln-1, ln},
      then ~l1 /\ ~l2 /\ ... /\ ~ln-1 --> ln.
     */
    List<PiecewiseLinearCaseSplit> getCaseSplits() const override;

    /*
      Check if the constraint's phase has been fixed.
    */
    bool phaseFixed() const override { return _isFixed; }

    /*
      If the constraint's phase has been fixed, get the (valid) case split.
      Not relevant for Sigmoid, should not call it phase isn't fixed.
    */
    PiecewiseLinearCaseSplit getValidCaseSplit() const override;

    /*
      Preprocessing related functions, to inform that a variable has been eliminated completely
      because it was fixed to some value, or that a variable's index has changed (e.g., x4 is now
      called x2). constraintObsolete() returns true iff and the constraint has become obsolote
      as a result of variable eliminations.
    */
    void eliminateVariable( unsigned variable, double fixedValue ) override;
    void updateVariableIndex( unsigned oldIndex, unsigned newIndex ) override;
    bool constraintObsolete() const override;

    /*
      Get the tightenings entailed by the constraint.
    */
    void getEntailedTightenings( List<Tightening> &tightenings ) const override;

    /*
      Dump the current state of the constraint.
    */
    void dump( String &output ) const override;

    /*
      For preprocessing: get any auxiliary equations that this
      constraint would like to add to the equation pool. In the ReLU
      case, this is an equation of the form aux = f - b, where aux is
      non-negative.
    */
    void addAuxiliaryEquations( InputQuery & inputQuery ) override;

    /*
      Ask the piecewise linear constraint to contribute a component to the cost
      function. If implemented, this component should be empty when the constraint is
      satisfied or inactive, and should be non-empty otherwise. Minimizing the returned
      equation should then lead to the constraint being "closer to satisfied".
    */
    void getCostFunctionComponent( Map<unsigned, double> &cost ) const override;

    /*
      Returns string with shape: sigmoid, _f, _b
    */
    String serializeToString() const override;

    /*
      Get the index of the B or F variable and their bounds
    */
    unsigned getB() const override { return _b; }
    unsigned getF() const override { return _f; }
    Point getLowerParticipantVariablesBounds() const override;
    Point getUpperParticipantVariablesBounds() const override;

    /*
     * Evaluate the concise function given point in the range
     */
    double evaluateConciseFunction(double x) const override { return FloatUtils::sigmoid(x); };

    /*
     * Evaluate the derivative of the concise function given point in the range
     */
    double evaluateDerivativeOfConciseFunction(double x) const override;


    /*
     * Evaluate the Inverse of the concise function given point in the range
    */
    double evaluateInverseOfConciseFunction(double y) const override { return FloatUtils::sigmoidInverse(y); };

    /*
     * Check if the concise function is convex function
    */
    ConvexType getConvexTypeInSegment(double x0, double x1) const override;

    /*
      Return true if and only if this piecewise linear constraint supports
      symbolic bound tightening.
    */
    bool supportsSymbolicBoundTightening() const override { return false; };

    /*
     * The constraint is active even if splited
     */
    bool isActive() const override { return _constraintActive; }

    /*
     *Turn the constraint on/off.
     */
    void setActiveConstraint( bool active ) override;
    

    /* For debugging propose */
    void LogFile(File *file = nullptr) { _logFile = file; };


private:

    unsigned _b, _f;
    bool _isBoundWereChanged;
    bool _haveEliminatedVariables;
    bool _isFixed;

    /* For debugging propose */
    IFile *_logFile = nullptr;
    int _iter_case_splits = 0;
    int _iter_fixes = 0;
    int _iter_satisfied = 0;
    int _restore = 0;
    void writePoint(double x, double y, bool isFix = false);
    void writeLimit(double lower, double upper, bool isB = false);
    void writeEquations(List<Equation> eqs);
    void dumpAssignment(double bValue, double fValue) const;
    void dumpUpperBound(const List<Equation> &refinements) const;
    void dumpSplits(const List<PiecewiseLinearCaseSplit> &splits) const;
    void dumpFixes(double bValue, double fValue, double sigmoidValue) const;
    void dumpRestore(bool isBefore) const;
    int sigmoid_num;

};


#endif //MARABOU_SIGMOIDCONSTRAINT_H
