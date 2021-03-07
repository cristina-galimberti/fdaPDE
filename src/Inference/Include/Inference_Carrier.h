#ifndef __INFERENCE_CARRIER_H__
#define __INFERENCE_CARRIER_H__

// HEADERS
#include "../../FdaPDE.h"
#include "../../Regression/Include/Mixed_FE_Regression.h"
#include "../../Regression/Include/Regression_Data.h"
#include "../../Lambda_Optimization/Include/Optimization_Data.h"
#include "../../Lambda_Optimization/Include/Solution_Builders.h"
#include "Inference_Data.h"

// *** Inference_Carrier Class ***
//! Carrier for Inference Data
/*!
 This class contains all the information needed by inference methods. All the needed objects are wrapped with pointers
 whith the exception of the smoohing parameter lambda and the functional fitted values f_hat
 \tparam InputHandler the type of regression problem needed to determine the MIxedFERegressionBase object type
*/
template<typename InputHandler>
class Inference_Carrier{
	private:
		// DATA:
		const OptimizationData * opt_data = nullptr;                    	//!< Pointer to the optimization data needed for the method
		const MixedFERegressionBase<InputHandler> * model = nullptr;    	//!< Pointer to the model data
		const InferenceData * inf_data = nullptr;				//!< Pointer to the inference data needed to perform inference

		// SYSTEM PARAMETERS
		const UInt n_obs; 							//!< Number of observations
		const UInt n_nodes;							//!< Number of nodes
		const Uint p;								//!< Number of covariates
		const Real var_res; 							//!< Variance of the residuals i n the optimal model

		Real lambda=0; 								//!< Optimal smothing parameter
  		const MatrixXr * Wp = nullptr;						//!< Pointer to the covariates matrix [size n_obs x n_covariates]
		const Diffusion<PDEParameterOptions::SpaceVarying> * Kp = nullptr; 	//!< Pointer to the nuisance matrix K [in the cases in which inference is allowed K is always a matrix]
		const SpMat * Psip = nullptr; 						//!< Pointer to location-to-nodes matrix [size n_obs x n_nodes]	
		const SpMat * Psi_tp = nullptr; 					//!< Pointer to the transpose of the location-to-nodes matrix [size n_nodes x n_obs]	
		const SpMat * R1p = nullptr;						//!< Pointer to R1 matrix [size n_nodes x n_nodes]
                const SpMat * R0p = nullptr;						//!< Pointer to R0 matrix [size n_nodes x n_nodes]
		const MtrixXr * Pp = nullptr;						//!< Pointer to P matrix [size ]
		const Eigen::PartialPivLU<MatrixXr> * WtW_decp = nullptr;		//!< Pointer to the LU decomposition of the WtW matrix
		const MatrixXr * Hp = nullptr;						//!< Pointer to the hat matrix [size n_covariates x n_covariates]
		const MatrixXr * Up = nullptr; 						//!< Pointer to the U matrix of the Woodbury decomposition of the system
		const MatrixXr * Vp = nullptr; 						//!< Pointer to the V matrix of the Woodbury decomposition of the system
		const SpMat * Ep = nullptr; 						//!< Pointer to the no-cov-matrix of the Woodbury decomposition of the system
		const Eigen::SparseLU<SpMat> * E_decp = nullptr;			//!< Pointer to the sparse LU decomposition for the no-cov-matrix of the Woodbury decomposition of the system
		const Eigen::PartialPivLU<MatrixXr> * G_decp = nullptr;			//!< Pointer to the LU decomposition of the G matrix of the Woodbury decomposition of the system
		
		// LOCAL VALUES
		const MatrixXv * beta_hatp = nullptr; 					//!< Pointer to the estimate of the betas for the optimal model
		const VectorXr * zp = nullptr;						//!< Pointer to the observations in the locations [size n_obs]
		const MatrixXr * z_hatp = nullptr; 					//!< Pointer to the fitted values in the locations [size n_obs]
		const MatrixXr * f_hatp = nullptr; 					//!< Pointer to the fitted values for the nonlinear part of the model in the locations [size n_obs]

		// SETTERS 								// Private because they will be used just by the constructor.
		inline void setOptData (const OptimizationData * opt_data_){opt_data = opt_data_;}			//!< Setter of opt_data \param opt_data_ new opt_data
		inline void setModel (const MixedFERegressionBase<InputHandler> * model_){model = model_;}		//!< Setter of model \param model_ new model
		inline void setInfData (const InferenceData * inf_data_){inf_data = inf_data_;}				//!< Setter of inf_data \param inf_data_ new inf_data

		inline void setN_obs (UInt n_obs){n_obs = n_obs_;}							//!< Setter of n_obs \param n_obs_ new n_obs
		inline void setN_nodes (UInt n_nodes_){n_nodes = n_nodes_;}						//!< Setter of n_nodes \param n_nodes_ new n_nodes
		inline void setp (UInt p_){p = p_;}									//!< Setter of p \param p_ new p
		inline void setVar_res (Real var_res_){var_res = var_res_;}						//!< Setter of var_res \param var_res_ new var_res

		inline void setLambda (Real lambda_){lambda = lambda_;}							//!< Setter of lambda \param lambda_ new lambda
		inline void setWp (const MatrixXr * Wp_){Wp = Wp_;}							//!< Setter of Wp \param Wp_ new Wp
		inline void setKp (const Diffusion<PDEParameterOptions::SpaceVarying> * Kp_){Kp = Kp_;}			//!< Setter of Kp \param Kp_ new Kp
		inline void setPsip (const SpMat * Psip_){Psip = Psip_;}						//!< Setter of Psip \param Psip_ new Psip
		inline void setPsi_tp (const SpMat * Psi_tp_){Psi_tp = Psi_tp_;}					//!< Setter of Psi_tp \param Psi_tp_ new Psi_tp
		inline void setR0p (const SpMat * R0p_){R0p = R0p_;}							//!< Setter of R0p \param R0p_ new R0p
		inline void setR1p (const SpMat * R1p_){R1p = R1p_;}							//!< Setter of R1p \param R1p_ new R1p
		inline void setPp (const MatrixXr * Pp_){Pp = Pp_;}							//!< Setter of Pp \param Pp_ new Pp
		inline void setWtW_decp (const Eigen::PartialPivLU<MatrixXr> * WtW_decp_){WtW_decp = WtW_decp_;}	//!< Setter of  WtW_decp \param  WtW_decp_ new  WtW_decp
		inline void setHp (const MatrixXr * Hp_){Hp = Hp_;}							//!< Setter of Hp \param Hp_ new Hp
		inline void setUp (const MatrixXr * Up_){Up = Up_;}							//!< Setter of Up \param Up_ new Up
		inline void setVp (const MatrixXr * Vp_){Vp = Vp_;}							//!< Setter of Vp \param Vp_ new Vp
		inline void setEp (const SpMat * Ep_){Ep = Ep_;}							//!< Setter of Ep \param Ep_ new Ep
		inline void setE_decp (const Eigen::SparseLU<SpMat> * E_decp_){E_decp = E_decp_;}			//!< Setter of E_decp \param E_decp_ new E_decp
		inline void setG_decp (const Eigen::PartialPivLU<MatrixXr> * G_decp_){G_decp = G_decp_;}		//!< Setter of G_decp \param G_decp_ new G_decp
		inline void setBeta_hatp (const MatrixXv * beta_hatp_){beta_hatp = beta_hatp_;}				//!< Setter of beta_hatp \param beta_hatp_ new beta_hatp
		inline void setZp (const VectorXr * zp_){zp = zp_;}							//!< Setter of zp \param zp_ new zp
		inline void setZ_hatp (const MatrixXr * z_hatp_){z_hatp = z_hatp_;}					//!< Setter of z_hatp \param z_hatp_ new z_hatp
		inline void setF_hatp (void){f_hatp = *z_hatp - (*Wp)*(*beta_hatp);}					//!< Setter of f_hatp new Wp

	public:
		// CONSTUCTORS
		Inference_Carrier()=default;			//The default constructor is just used to initialize the object. All the pointer are set to nullptr, lambda is set to 0
		Inference_Carrier(const InputHandler * Regression_Data_, const MixedFERegressionBase<InputHandler> * model_, const output_Data * out_regression, const InferenceData inf_data_*); //Main constructor of the class

		// GETTERS
		inline const OptimizationData * opt_data getOptData (void) const {return opt_data;}  			//!< Getter of opt_data \return opt_data
		inline const MixedFERegressionBase<InputHandler> * getModel (void) const {return model;} 		//!< Getter of model \return model
		inline const InferenceData * getInfData (void) const {return inf_data;}					//!< Getter of inf_data \return inf_data

		inline UInt getN_obs (void) const {return n_obs;} 							//!< Getter of n_obs \return n_obs
		inline UInt getN_nodes (void) const {return n_nodes;} 							//!< Getter of n_nodes \return n_nodes
		inline UInt getp (void) const {return p;} 								//!< Getter of p \return p
		inline Real getVar_res (void) const {return var_res;} 							//!< Getter of var_res \return var_res

		inline Real getLambda (void) const {return lambda;} 							//!< Getter of lambda \return lambda
		inline const MatrixXr * getWp (void) const {return Wp;} 						//!< Getter of Wp \return Wp
                inline const MatrixXr * getKp (void) const {return Kp;} 						//!< Getter of Kp \return Kp
		inline const SpMat * getPsip (void) const {return Psip;} 						//!< Getter of Psip \return Psip
		inline const SpMat * getPsi_tp (void) const {return Psi_tp;} 						//!< Getter of Psi_tp \return Psi_tp
		inline const SpMat * getR0p (void) const {return R0p;} 							//!< Getter of R0p \return R0p
		inline const SpMat * getR1p (void) const {return R1p;} 							//!< Getter of R1p \return R1p
		inline const MatrixXr * getPp (void) const {return Pp;} 						//!< Getter of Pp \return Pp
		inline const Eigen::PartialPivLU<MatrixXr> * getWtW_decp (void) const {return WtW_decp;} 		//!< Getter of WtW_decp \return WtW_decp
		inline const MatrixXr * getHp (void) const {return Hp;} 						//!< Getter of Hp \return Hp
		inline const MatrixXr * getUp (void) const {return Up;} 						//!< Getter of Up \return Up
		inline const MatrixXr * getVp (void) const {return Vp;} 						//!< Getter of Vp \return Vp
		inline const MatrixXr * getEp (void) const {return Ep;} 						//!< Getter of Ep \return Ep
		inline const Eigen::SparseLU<SpMat> * getE_decp (void) const {return E_decp;} 				//!< Getter of E_decp \return E_decp
		inline const Eigen::PartialPivLU<MatrixXr> * getG_decp (void) const {return G_decp;} 			//!< Getter of G_decp \return G_decp
		inline const MatrixXv * getBeta_hatp (void) const {return beta_hatp;} 				        //!< Getter of beta_hatp \return beta_hatp
		inline const VectorXr * getZp (void) const {return zp;} 						//!< Getter of zp \return zp
		inline const MatrixXr * getZ_hatp (void) const {return z_hatp;} 					//!< Getter of z_hatp \return z_hatp
		inline const MatrixXr * getF_hatp (void) const {return f_hatp;} 					//!< Getter of f_hatp \return f_hatp



};



#endif